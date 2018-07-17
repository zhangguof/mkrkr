#ifndef _OBJECT_2D_H_
#define _OBJECT_2D_H_
#include "GLDevice.hpp"
#include "gltexture.hpp"
#include "glm/vec3.hpp"

// static GLfloat vertices[] = {
//         // 第一个三角形       贴图坐标
//         -1.0f,  1.0f, 0.0f, 0.0f,1.0f,   // 右上角
//         1.0f,  -1.0f, 0.0f, 1.0f,0.0f,  // 右下角
//         -1.0f, -1.0f, 0.0f, 0.0f,0.0f, // 左上角
//         // 第二个三角形
//         -1.0f,  1.0f, 0.0f, 0.0f,1.0f,  
//         1.0f,   1.0f, 0.0f, 1.0f,1.0f,
//         1.0f,  -1.0f, 0.0f, 1.0f,0.0f,
//     };

typedef std::shared_ptr<VAOMGR> tPtrVao;
typedef std::shared_ptr<SHADER> tPtrShader;
typedef std::shared_ptr<GLTexture> tPtrTexture;

//vertice with pos and texture coord;
// struct Vertice
// {
// 	float data[5];
// 	glm::vec3 pos;
// 	glm::vec2 tex_coord;
// 	Vertice(float d[])
// 	{
// 		memcpy(data,d,sizeof(float)*5);
// 		pos = glm::vec3(d[0],d[1],d[2]);
// 		tex_coord = glm::vec2(d[3],d[4]);
// 	}
// };


struct VerticeBuffer
{
	std::vector<float> data;
	int n;
	VerticeBuffer(){
		n = 0;
		data.resize(4*5);
	}
	void addVertice(float* pos,int size)
	{
		if(n + size > data.size())
		{
			data.resize(size+n);
		}
		memcpy(data.data()+n,pos,sizeof(float)*size);
		n += size;
	}
	float* get_data()
	{
		return data.data();
	}
	int get_size()
	{
		return n*sizeof(float);
	}
};

class BaseObject2d;
typedef std::shared_ptr<BaseObject2d> tPtrObj2d;

class BaseObject2d: public std::enable_shared_from_this<BaseObject2d>
{
public:
	// std::vector<Vertice> vertices;
	VerticeBuffer vertices;
	int vert_num;
	tPtrVao pvao;
	tPtrShader pshader; 
	tPtrTexture ptexture;
	glm::mat4 transform;
	glm::mat4 ortho;
	// BaseObject2d* base;
	tPtrObj2d base;
	std::vector<tPtrObj2d> nodes;


void set_trans(glm::mat4& _tans)
{
	transform = _tans;
}

virtual void draw();
void do_draw();//real do draw
void addVertice(float pos[5],int size);
void add_node(tPtrObj2d node);
void init();
	BaseObject2d(int n=3,tPtrObj2d pbase=nullptr);
	~BaseObject2d();
};

class RectObj:public BaseObject2d
{
public:
	int left,top;
	int width,height;
public:
	RectObj(tPtrObj2d pbase = nullptr);
	RectObj(int l,int t,int w,int h,tPtrObj2d pbase = nullptr);
	void init();
};


#endif