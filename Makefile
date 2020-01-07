flag=g++ -g -Wall 

src=$(shell ls src/*.cpp)\
	$(shell ls ../lib/src/*cpp)

tmp=$(subst src,obj,$(src))
obj=$(subst .cpp,.o,$(tmp))


client: $(obj)
	$(flag) $(obj) -o bin/client
	bin/client


obj/%.o: src/%.cpp
	$(flag) -c $< -o $@


../lib/obj/%.o: ../lib/src/%.cpp
	$(flag) -c $< -o $@


clean:
	rm -rf obj/*.o
	rm -rf bin/client
	rm -rf ../lib/obj/*.o
	rm -f hello.*
