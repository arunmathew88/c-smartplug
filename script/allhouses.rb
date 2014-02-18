sync_port = 5556
data_port = 4000

(0..39).each { |house|
	system("./bin/house #{house} localhost #{sync_port} #{data_port+house} > out#{house} &")
}
