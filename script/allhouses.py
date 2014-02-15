import os

sync_port = 5556
default_data_port = 4000

for i in range(40):
	port_number = default_data_port + i
	command = "./bin/house_process " + str(i) + " localhost " + str(sync_port) + " " + str(port_number) + " & " + " > out" + str(port_number)
	os.system(command)
