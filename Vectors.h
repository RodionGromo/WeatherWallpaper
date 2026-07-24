#pragma once

#include <SDL3/SDL_rect.h>

#undef RGB;

static class Vectors
{
public:
	class Vector2 {
	private:
		int x;
		int y;
	public:
		Vector2() { set(0, 0); }
		Vector2(int x, int y) { set(x, y); }
		int X() const { return x; }
		int Y() const { return y; }
		void set(int x, int y) {
			this->x = x; this->y = y;
		}
	};

	class Vector3 {
	private:
		int x;
		int y;
		int z;
	public:
		Vector3() { set(0, 0, 0); }
		Vector3(int x, int y, int z) { set(x, y, z); }
		int X() const { return x; }
		int Y() const { return y; }
		int Z() const { return z; }
		void set(int x, int y, int z) {
			this->x = x; this->y = y; this->z = z;
		}
	};

	class RGB : public Vector3 {
	public:
		RGB(int r, int g, int b) { set(r,g,b); }
		int R() const { return X(); }
		int G() const { return Y(); }
		int B() const { return Z(); }
		void set(int r, int g, int b) {
			this->Vector3::set(r,g,b);
		}
	};

	class Rect {
	private:
		Vector2 position;
		Vector2 dimensions;
	public:
		Rect(Vector2 pos, Vector2 size) {
			this->position = pos; this->dimensions = size;
		}
		Rect(int x, int y, int w, int h) {
			this->position = Vector2(x, y);
			this->dimensions = Vector2(w, h);
		}
		Vector2 getPosition() const { return this->position; }
		Vector2 getSize() const { return this->dimensions; }
		SDL_FRect getSDLRect() const {
			return { (float)position.X(), (float)position.Y(), (float)dimensions.X(), (float)dimensions.Y() };
		}
	};
};

