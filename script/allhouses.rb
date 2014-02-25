sync_port = 5556
data_port = 4000

(0..39).each { |house|
	system("./bin/house #{house} 10.105.3.108 #{sync_port} 10.105.3.108 #{data_port+house} > out#{house} &")
}
