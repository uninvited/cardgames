CXXFLAGS =-I. -std=c++17 -Wall -O2
CC-COMMAND=g++ -c -o $@ $< $(CXXFLAGS) $(LIBS)

OBJ = main.o \
      game.o \
      player.o \
      serialize.o \
      common/card_traits.o \
      logging/logging.o \
      strategy/random_strategy.o \
      strategy/min_card_strategy.o \
      strategy/helper.o \

%.o: %.cpp
	$(CC-COMMAND)

common/%.o:%.cpp
	$(CC-COMMAND)

logging/%.o: %.cpp
	$(CC-COMMAND)

durak: $(OBJ)
	g++ -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o */*.o ./durak

