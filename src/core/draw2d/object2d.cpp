//2d object 
#include "object2d.hpp"
#include "GLDevice.hpp"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaders.hpp"
const int defalut_vertice_num = 6;

const int  defalut_vertice_size = 5;
static GLfloat default_rect_vertices[] = {
        // first triangle      textrue coord
        -1.0f,  1.0f, 0.0f, 0.0f,1.0f,   // 右上角
        1.0f,  -1.0f, 0.0f, 1.0f,0.0f,  // 右下角
        -1.0f, -1.0f, 0.0f, 0.0f,0.0f, // 左上角
        // second triangle
        -1.0f,  1.0f, 0.0f, 0.0f,1.0f,  
        1.0f,   1.0f, 0.0f, 1.0f,1.0f,
        1.0f,  -1.0f, 0.0f, 1.0f,0.0f,
    };



BaseObject2d::BaseObject2d(int n,tPtrObj2d pbase)
{
	pvao = nullptr;
	pshader = nullptr;
	ptexture = nullptr;
	vert_num = n;
	transform = glm::mat4(1.0f);
	ortho = glm::mat4(1.0f);
	base  = pbase;
	nodes.clear();

}

BaseObject2d::~BaseObject2d()
{
	// vertices.clear();
}

void BaseObject2d::addVertice(float pos[5],int size)
{

	vertices.addVertice(pos,size);
}

void BaseObject2d::init()
{
	//before draw ,after add points;
	pvao = std::make_shared<VAOMGR>(vert_num);
	if(base==nullptr)
		pshader = std::make_shared<SHADER>(object2d_vs_sharder,
				 object2d_fs_sharder);
	else
		pshader = base->pshader;
	// pshader->add_uniform_mat("transform");
	// pshader->init_all_uniform();
	// pshader->use();
	// trans = glm::translate(glm::mat4(1.0f),
	// 		glm::vec3(1.0,-1.0,0.0f));
	// pshader->set_mat("transform",trans);

	//gen vao vbo
	pvao->gen_vao_vbo(vertices.get_data(),vertices.get_size());
	if(base)
	{
		base->add_node(shared_from_this());
	}

	// printf("BaseObject2d init!:%d\n",vertices.size());
}


void BaseObject2d::do_draw()
{
	for(auto pnode:nodes)
	{
		pnode->do_draw();
	}
	// printf("do draw!!\n");
	if(pshader)
	{
        // get matrix's uniform location and set matrix
        pshader->use();
        pshader->set_mat("transform",transform);
        pshader->set_mat("ortho",ortho);
	}
	if(ptexture)
	{
		ptexture->update_texture();
	}
	if(pvao)
	{
		pvao->bind();
		pvao->draw();
		// printf("draw vao!!\n");
	}
}

void BaseObject2d::draw()
{
	if(base)
	{
		base->draw();
		return;
	}
	do_draw();
}

void BaseObject2d::add_node(tPtrObj2d node)
{
	nodes.push_back(node);
}

//Rectangle object
RectObj::RectObj(tPtrObj2d pbase):BaseObject2d(defalut_vertice_num,pbase)
{
	left = top = width = height = 0;
	init();

}

RectObj::RectObj(int l, int t, int w,int h,
				 tPtrObj2d pbase):BaseObject2d(defalut_vertice_num,pbase)
{
	left = l;
	top = t;
	width = w;
	height = h;
	init();

}

static void printf_mat4(glm::mat4& m4)
{
	for(int i=0;i<4;++i)
	{
		for(int j=0;j<4;++j)
		{
				printf("%f,",m4[j][i]);
		}
		printf("\n");
	}
}
static void printf_pos(glm::mat4& m4,glm::vec3 pos)
{
	glm::vec4 p = m4*glm::vec4(pos,1.0f);
	printf("(%f,%f,%f)\n",p.x,p.y,p.z);
}

void RectObj::init()
{
	float w = static_cast<float>(width);
	float h = static_cast<float>(height);
// static GLfloat default_rect_vertices[] = {
//         // first triangle      textrue coord
//         -1.0f,  1.0f, 0.0f, 0.0f,1.0f,   // 右上角
//         1.0f,  -1.0f, 0.0f, 1.0f,0.0f,  // 右下角
//         -1.0f, -1.0f, 0.0f, 0.0f,0.0f, // 左上角
//         // second triangle
//         -1.0f,  1.0f, 0.0f, 0.0f,1.0f,  
//         1.0f,   1.0f, 0.0f, 1.0f,1.0f,
//         1.0f,  -1.0f, 0.0f, 1.0f,0.0f,
//     };
	GLfloat vertices[] = {
        // first triangle      textrue coord
        0.0f,  	 h, 	0.0f, 0.0f,1.0f,   // 右上角
        w,   	 0.0f,  0.0f, 1.0f,0.0f,  // 右下角
        0.0f, 	 0.0f,  0.0f, 0.0f,0.0f, // 左上角
        // second triangle
        0.0f,    h, 	0.0f, 0.0f,1.0f,  
        w,   	 h, 	0.0f, 1.0f,1.0f,
        w,   	 0.0f,  0.0f, 1.0f,0.0f,
    };
	// GLfloat* vertices = default_rect_vertices;
    int vertice_size = defalut_vertice_size;
	// int n = sizeof(vertices)/sizeof(float)/vertice_size;
	int n=6;
	assert(n==6);
	for(int i=0;i<n;++i)
	{
		addVertice(vertices+i*vertice_size,vertice_size);
	}
	// ortho = glm::mat4(1.0f);

	BaseObject2d::init();

	if(base)
	{
		transform = glm::translate(base->transform,glm::vec3(left,top,0.0f));//to world coord.
		ortho = base->ortho;
	}
	else
	{
		ortho = glm::ortho(0.0f, w, 0.0f, h, -100.0f, 100.0f);
		// printf_mat4(ortho);
		// printf_mat4(transform);
		// glm::vec4 pos(w/2,h/2,0,1.0);

		// pos = ortho * transform * pos;
		// printf("(%f,%f)\n",pos.x,pos.y);
		auto mat = ortho * transform;
		// printf("printf_pos!!1\n");
		// printf_pos(mat,glm::vec3(0,h,0));
		// printf_pos(mat,glm::vec3(w,0,0));
		// printf_pos(mat,glm::vec3(0.0f,0.0f,0.0f));
		// printf_pos(mat,glm::vec3(w,h,0.0f));
		// printf_pos(mat,glm::vec3(w,0.0f,0.0f));
	}
}



//test draw
std::shared_ptr<BaseObject2d> pobj = nullptr;

extern SDL_Window* gWindow;
static uint32_t last_tick = 0;
static float v=0.05f;
static void update(uint32_t now)
{
	// auto time_now = SDL_GetTicks();
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
	if(now - last_tick > 30)
	{
		// glm::mat4 new_trans(1.0f);
		// pobj->transform = glm::translate(glm::mat4(1.0f),
		// 		glm::vec3(0.1+v,-0.1-v,0.0f));
		
		v += 0.05f;

		last_tick = now;
	}
	if(pobj)
	{
		// printf("update!! test draw\n");
		pobj->draw();
	}
	SDL_GL_SwapWindow( gWindow );
}


void test_object_2d_draw()
{
	pobj = std::make_shared<RectObj>(0,0,1024,768);

	regist_update(update);
}





