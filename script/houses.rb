port = 5556

(0..39).each { |house|
	system("./bin/client #{house} localhost #{port} > #{house}.out &")
}
