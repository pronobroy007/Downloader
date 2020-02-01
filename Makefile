flag=g++ -g -Wall 
lib=-L/usr/local/ssl/lib -lssl -lcrypto  
project=../bin/downloader

src=$(shell ls src/*.cpp)\
	$(shell ls ../lib/src/*cpp)

tmp=$(subst src,obj,$(src))
obj=$(subst .cpp,.o,$(tmp))


client: $(obj)
	$(flag) $(obj) -o $(project) $(lib)
	$(project)


obj/%.o: src/%.cpp
	$(flag) -c $< -o $@ $(lib)


../lib/obj/%.o: ../lib/src/%.cpp
	$(flag) -c $< -o $@ $(lib)


clean:
	rm -rf obj/*.o
	rm -rf $(project)
	rm -rf ../lib/obj/*.o
	rm -rf download/*
