port = 5556

(0..39).each { |house|
	system("./bin/house #{house} 10.105.3.108 #{port} > #{house}.out &")
}
