# requires GNU Make ver 4.3

objects = $(obj_dir)/main.o $(obj_dir)/datapath.o $(obj_dir)/fetch.o $(obj_dir)/decoder.o
headers = $(header_dir)/datapath.h $(header_dir)/fetch.h $(header_dir)/decoder.h

header_dir = header
src_dir = src
obj_dir = object

risc_simulator.exe : $(objects)
	g++ -o risc_simulator.exe $(objects)



$(obj_dir)/main.o : $(src_dir)/main.cpp $(headers) 
	g++ -g -c $(src_dir)/main.cpp -o $(obj_dir)/main.o

$(obj_dir)/datapath.o : $(src_dir)/datapath.cpp $(header_dir)/datapath.h
	g++ -g -c $(src_dir)/datapath.cpp -o $(obj_dir)/datapath.o

$(obj_dir)/fetch.o : $(src_dir)/fetch.cpp $(header_dir)/fetch.h $(header_dir)/datapath.h
	g++ -g -c $(src_dir)/fetch.cpp -o $(obj_dir)/fetch.o

$(obj_dir)/decoder.o : $(src_dir)/decoder.cpp  $(header_dir)/decoder.h $(header_dir)/datapath.h
	g++ -g -c $(src_dir)/decoder.cpp -o $(obj_dir)/decoder.o

.PHONY : clean

clean:
	rm -f $(obj_dir)/*.o risc_simulator.exe