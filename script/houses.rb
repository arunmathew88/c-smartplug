port = 5556

(0..39).each { |house|
	system("./bin/client #{house} 127.0.0.1 #{port} > #{house}.out &")
}
