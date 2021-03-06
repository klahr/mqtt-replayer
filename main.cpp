#include <iostream>
#include <MQTTClient.h>
#include <cstring>
#include <fstream>
#include <ctime>
#include <unistd.h>
#include <sys/time.h>
#include "json11.hpp"

// export LD_LIBRARY_PATH=/usr/local/lib

struct Message {
	int Id;
	std::string Topic;
	std::string Payload;
	int Time;
	bool Retained;
};

long long getEpochTime() {
	timeval tv;
	gettimeofday(&tv, nullptr);
	long long millisecondsSinceEpoch =
		(long long)(tv.tv_sec) * 1000 +
		(long long)(tv.tv_usec) / 1000;
	return millisecondsSinceEpoch;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("./replayer mqtt_messages.log");
		return 0;
	}

	MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
	opts.keepAliveInterval = 20;
	opts.cleansession = 1;

	MQTTClient client;
	MQTTClient_create(&client, "tcp://localhost:1883", "ClientID", MQTTCLIENT_PERSISTENCE_NONE, NULL);
	int rc;
	if ((rc = MQTTClient_connect(client, &opts)) != MQTTCLIENT_SUCCESS) {
		printf("Failed to connect, return code %d\n", rc);
		return -1;
	}

	std::ifstream infile(argv[1]);
	std::string line;
	MQTTClient_deliveryToken token;
	long long start = getEpochTime();
	while (std::getline(infile, line)) {
		if (line[0] == '#') {
			continue;
		}
		std::string err;
		json11::Json json = json11::Json::parse(line.c_str(), err);
		Message message;
		message.Id = json["id"].int_value();
		message.Payload = json["payload"].string_value();
		message.Time = json["timestamp"].int_value();
		message.Topic = json["topic"].string_value();
		message.Retained = json["retained"].bool_value();

		long long now = getEpochTime();
		long long elapsed = now - start;
		if (elapsed < message.Time) {
			int timeToSleep = message.Time - elapsed;
			usleep(timeToSleep * 1000);
		}
		//printf("Publishing message: %s", line.c_str());

		MQTTClient_publish(client, message.Topic.c_str(), message.Payload.length(), const_cast<void*>(static_cast<const void*>(message.Payload.c_str())), 1, message.Retained ? 1 : 0, &token);
	}

	MQTTClient_disconnect(client, 0);

	return 0;
}
