brokers = []
houses = []
filename = []
username = ""
password = ""
eval File.open('script/common.rb').read
require 'shellwords'

system("mkdir ~/debsresults")
brokers.each.with_index { |b, i|
	system("sshpass -p #{password.shellescape} scp -r #{username}@#{b}:/home/#{username}/debsresults ~/")
}

houses.each.with_index{ |h, i|
	system("sshpass -p #{password.shellescape} scp -r #{username}@#{h}:/home/#{username}/debsresults ~/")
}

time = Time.now.to_i
root = "/home/#{username}/debsresults"
expFile = Dir.entries(root).select {|entry| File.directory? File.join(root,entry) and !(entry =='.' || entry == '..') }
expFile.each { |e|
	ts = Dir.entries("#{root}/#{e}").select {|entry| File.directory? File.join("#{root}/#{e}",entry) and !(entry =='.' || entry == '..') }
	ts.each {|run|
		puts "#{root}/#{e}/#{run}"
		system("cd #{root}/#{e}/#{run}; cat *.out |grep Latency |cut -d '=' -f2 |cut -d' ' -f2 > allLatency");
		system("cd #{root}/#{e}/#{run}; R -q -e \"x <- read.csv('allLatency', header = F); summary(x); sd(x[ , 1]); quantile(x[ ,1], c(.10,.90));\" > LatStat");
		system("cd #{root}/#{e}/#{run}; cat LatStat |grep Mean |cut -d':' -f2 |tr -d ' ' >summary");
		system("cd #{root}/#{e}/#{run}; cat LatStat |grep -A1 \"10%\" |grep -v \"10%\" >>summary");
                system("cd #{root}/#{e}/#{run}; cat LatStat |grep \"\\[1\\]\"|cut -d ' ' -f2 >> summary");
		system("cd #{root}/#{e}/#{run}; echo `cat broker |grep System |cut -d ':' -f2` + `cat broker |grep User |cut -d ':' -f2` |bc >>summary");
                system("cd #{root}/#{e}/#{run}; echo `cat broker |grep Elapsed |cut -d ':' -f5-` >>summary");
                system("cd #{root}/#{e}/#{run}; cat broker |grep Throughput  |cut -d '=' -f2 |cut -d' ' -f2 >allThroughput");
                system("cd #{root}/#{e}/#{run}; R -q -e \"x <- read.csv('allThroughput', header = F); summary(x); sd(x[ , 1]); quantile(x[ ,1], c(.10,.90));\">ThroughStat");
                system("cd #{root}/#{e}/#{run}; cat ThroughStat |grep Mean |cut -d':' -f2 |tr -d ' ' >> summary");
                system("cd #{root}/#{e}/#{run}; cat ThroughStat |grep -A1 \"10%\" |grep -v \"10%\" >> summary");
                system("cd #{root}/#{e}/#{run}; cat ThroughStat |grep \"\\[1\\]\"|cut -d ' ' -f2 >> summary");
		system("echo #{e} #{run} `cat #{root}/#{e}/#{run}/summary` |sed 's/ /,/g' >> #{root}/summary.#{time}")
	}
}

