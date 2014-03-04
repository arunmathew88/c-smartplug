brokers = []
houses = []
filename = []
username = ""
password = ""
eval File.open('script/common.rb').read
require 'shellwords'

#Arguments are i=[0-2] j=[0-2]. 
#	i is the index of file to be used. 0,1,2 corresponds to 40,20,10 respectively.
#	j is the number of house VMs to be used 0,1,2 corresponds to 1,2,4

time = Time.now.to_i
j = ARGV[0].to_i
k = ARGV[1].to_i
brokers.each.with_index { |b, i|
        system("sshpass -p #{password.shellescape} ssh #{username}@#{b} \"mkdir -p debsresults/#{filename[j]}/#{time}; cd c-smartplug; /usr/bin/time -v ./bin/broker #{(2**(2-j))*10} /disk/#{filename[j]} 10000000 > /home/#{username}/debsresults/#{filename[j]}/#{time}/broker 2>&1 & \"")
}

houses.each.with_index{ |h, i|
        system("sshpass -p #{password.shellescape} ssh #{username}@#{h} \"mkdir -p debsresults/#{filename[j]}/#{time}; cd c-smartplug; ruby script/houses.rb #{i*40/(2**j*2**k)} #{(i+1)*40/(2**j*2**k)-1} #{brokers[i/4]} /home/#{username}/debsresults/#{filename[j]}/#{time} \"") if i<(2**k)
}

