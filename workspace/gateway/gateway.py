#!/usr/bin/python

from serial import Serial
from time import sleep
import json
from configparser import ConfigParser
import argparse
import logging.handlers
import paho.mqtt.client as mqtt

class TinkwiseGateway(object):
	def __init__(self):
		self._logger = logging.getLogger()
		self._logger.setLevel(logging.INFO)
		self._logger.addHandler(logging.handlers.SysLogHandler(address='/dev/log'))

		parser = argparse.ArgumentParser(description='tinkwise gateway')
		parser.add_argument('-c','--config_file', type=str, dest='config_file', default='/etc/tinkwise.conf')
		args = parser.parse_args()
		
		config = ConfigParser()
		config.read(args.config_file)
		connection_type = config.get('connection', 'type')
		connection_file = config.get('connection', 'file')
		

		self._client = mqtt.Client()
		self._client.connect("localhost")
		self._client.loop_start()
		
		if connection_type == 'file':
			self._input_file = open(connection_file)
		elif connection_type == 'serial':
			try:
				self._input_file = Serial(port=connection_file, baudrate=115200)
			except Exception as e:
				self._logger.critical('failed to open serial port ' + connection_file + ':' + e.message)
				raise
			
	def run(self):
		while True:
			line = self._input_file.readline().decode("utf-8")
			if line == '':
				sleep(1)
				continue
			self._logger.info('processing ' + line)
			try:
				sample = json.loads(line)
			except Exception as e:
				self._logger.error('could not parse JSON string ' + line)
				self._logger.error(str(e))
				continue
			if not 'node' in sample:
				self._logger.error('no node found in JSON string' + line)
				continue
			node_index = int(sample['node'])
			self._logger.info('node ' + str(node_index))
			
			topic = 'tinkwise/nodes/' + str(node_index)
			
			self._client.publish(topic, payload=line)


if __name__ == "__main__":
	gateway = TinkwiseGateway()
	gateway.run()
