port = 5556

((ARGV[0].to_i)..(ARGV[1].to_i)).each { |house|
	system("./bin/house #{house} #{ARGV[2]} #{port} > #{house}.out &")
}
