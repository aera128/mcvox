
#include <color/data.hpp>
#include <color/texture.hpp>

namespace color {

texture::texture(const std::string& filename) {
	load(filename);
}

texture::texture() : cx(0), cy(0), pixels(0) {
}

void texture::load(const std::string& filename) {
	this->image.read(filename);

	Magick::Geometry g = this->image.boundingBox();
	this->cx = g.width();
	this->cy = g.height();

	this->pixels = this->image.getConstPixels(0, 0, this->cx, this->cy);
}

unsigned int texture::width() const {
	return this->cx;
}

unsigned int texture::height() const {
	return this->cy;
}

color::value texture::texel(double u, double v) const {
	double fx = double(this->cx) * u;
	double fy = double(this->cy) * v;

	int    xs[2] = { int(floor(fx)),       int(ceil(fx)) };
	double xw[2] = { 1.0 - (u - floor(u)), 1.0 - (ceil(u) - u) };
	int    ys[2] = { int(floor(fy)),       int(ceil(fy)) };
	double yw[2] = { 1.0 - (v - floor(v)), 1.0 - (ceil(v) - v) };

	color::value cs[] = {
		texel(xs[0], ys[0]),
		texel(xs[1], ys[0]),
		texel(xs[0], ys[1]),
		texel(xs[1], ys[1])
	};

	double ws[] = {
		xw[0] * yw[0],
		xw[1] * yw[0],
		xw[0] * yw[1],
		xw[1] * yw[1]
	};

	return color::wavgc<4>(cs, ws);
}

color::value texture::texel(int tx, int ty) const {
	if (this->pixels) {
		tx = tx % this->cx;
		ty = this->cy - (ty % this->cy);
		const Magick::PixelPacket* p = this->pixels + tx + ty * this->cx;
		return color::make(p->red, p->green, p->blue, 0xff - p->opacity);
	} else {
		return color::make(0xff, 0xff, 0xff, 0xff);
	}
}

}

