begin_port = 4000

(0..39).each.with_index { |house, index|
	port = begin_port+index
	print system("./bin/house #{house} #{port} &")
	puts " house id: #{house}, port: #{port}"
}
