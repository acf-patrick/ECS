#include <core.h>
#include <box2d/box2d.h>
#include <yaml-cpp/yaml.h>
#include <SDL2/SDL_image.h>

using Script = Component::script;

class Controller : public Script
{
public:
	const float MtoPX = 80.0f;
	const float timeStep = 1/60.0f;
	b2World world;
	b2Body* body;
	Uint32 lastTick;

	Controller(VectorF gravity) :
		world(b2Vec2(gravity.x, gravity.y))
	{
		event.listen(Input.QUIT, [](Entity& entity)
		{
			APP->quit();
		});

		// Dynamic body
		b2BodyDef def;
		def.type = b2_dynamicBody;
		def.position.Set(400/MtoPX, 100/MtoPX);
		def.angle = b2_pi/6;
		body = world.CreateBody(&def);

		b2FixtureDef fdef;
		fdef.density = 1.0f;
		fdef.friction = 0.3f;
		fdef.restitution = 0.4f;
		b2PolygonShape shape;
		shape.SetAsBox(32/MtoPX, 32/MtoPX);
		fdef.shape = &shape;
		body->CreateFixture(&fdef);

		// Ground
		b2BodyDef groundBodyDef;
		groundBodyDef.position.Set(400/MtoPX, 400/MtoPX);
		groundBodyDef.type = b2_staticBody;
		auto ground = world.CreateBody(&groundBodyDef);
		
		b2PolygonShape groundShape;
		groundShape.SetAsBox(300/MtoPX, 10/MtoPX);
		ground->CreateFixture(&groundShape, 0.0f);
	}

	void Update() override
	{
		world.Step(timeStep, 8, 3);

		auto pos = body->GetPosition();
		auto& t = get<Component::transform>();
		t.position.x = pos.x*MtoPX;
		t.position.y = pos.y*MtoPX;
		t.rotation = body->GetAngle()*180/b2_pi;
	}

	void Render() override
	{
		Renderer->submit([&](SDL_Renderer* renderer)
		{
			SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
			SDL_Rect rect = { 100, 390, 600, 20 };
			SDL_RenderDrawRect(renderer, &rect);
		});
	}
};

class FollowMouseBehavior : public Script
{
	bool follow = false;
public:
	FollowMouseBehavior()
	{
		event.listen(Input.MOUSE_BUTTON_UP, [&](Entity& entity)
		{
			follow = !follow;
		});
	}

	void Update() override
	{
		if (follow)
		{
			auto& cameraPos = get<Component::camera>().destination;
			auto rs = Renderer->getSize();
			auto mouse = Input.mouse.getPosition();
			cameraPos.x = mouse.x/float(rs.x);
			cameraPos.y = mouse.y/float(rs.y);
		}
	}
};

class MySerializer : public Serializer
{
public:
	void deserializeEntity(YAML::Node& node, Entity& entity) override
	{
		Serializer::deserializeEntity(node, entity);
		auto c = node["Controller"];
		if (c)
		{
			VectorF gravity(0, 10.0f);
			float timeStep = 1/60.0f;
			if (c["Gravity"])
				gravity = c["Gravity"].as<VectorF>();
			entity.attach<Controller>(gravity);
		}
		c = node["FollowMouseBehavior"];
		if (c)
		{
			entity.attach<FollowMouseBehavior>();
		}
	}
};
Serializer* Application::serializer = new MySerializer;