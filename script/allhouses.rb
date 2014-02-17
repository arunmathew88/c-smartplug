sync_port = 5556
data_port = 5555

(0..40).each { |house|
	system("./bin/house #{house} localhost #{sync_port} #{data_port} > out#{house} &")
}
