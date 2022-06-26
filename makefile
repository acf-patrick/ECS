.SUFFIXE:

CXX 	= 	g++-10
CFLAGS 	= 	-std=c++17

CORE 	= 	core/ecs/component/manager.cpp\
			core/ecs/component/camera.cpp\
			core/ecs/component/sprites.cpp\
			core/ecs/component/tilemap/tilemap.cpp\
			core/ecs/component/tilemap/drawer.cpp\
			core/ecs/entity/entity.cpp\
			core/ecs/entity/container.cpp\
			core/application/application.cpp\
			core/entry/entry.cpp\
			core/scene/scene.cpp\
			core/scene/sceneManager.cpp\
			core/renderer/renderer.cpp\
			core/event/event.cpp\
			core/event/listner.cpp\
			core/util/blur/blur.cpp\
			core/texture/texture.cpp\
			core/serializer/serializer.cpp
 

CLIENT 	= 	test.cpp

HEADER 	=  	-I3rd-party/ -Icore/
LIBS 	= 	libs/

SDL 	= 	-lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
YAML 	= 	-lyaml-cpp
BOX2D =	-lbox2d
# TMX 	= 	-lxml2 -lz -ltmx

SRC 	= 	$(CORE) $(CLIENT)

ifeq ($(OS), Windows_NT)
	# fill
else
	L = $(SDL) -L$(LIBS)linux/ $(TMX) $(YAML) $(BOX2D)
endif

all : obj
	@echo "... Linkage ..."
	$(CXX) obj/* $(L) -o bin

obj: $(SRC)
	@echo "... Compiling ..."
	@mkdir -p obj
	$(CXX) $(CFLAGS) -c $? $(HEADER)
	@mv *.o obj/

clean:
	@rm -f bin *.o -r obj/

rebuild:
	@make clean; make