begin_port = 4000

(0..39).each.with_index { |house, index|
	port = begin_port+index
	system("./bin/house_process #{house} #{port} &")
}
