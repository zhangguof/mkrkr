
#include "GLDevice.hpp"
#include "shaders.hpp"

#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
////do something
const char* defalut_vs_shader = simple_es_vs_shader;
const char* defalut_fs_shader = simple_es_fs_shader;
#else
const char* defalut_vs_shader = simple_vs_shader;
const char* defalut_fs_shader = simple_fs_shader;

#endif


extern SDL_Window* gWindow;
// extern const char* simple_vs_shader;
// extern const char* simple_fs_shader;
// const char* defalut_vs_shader = NULL;
// const char* defalut_fs_shader = NULL;

const char* VERTEX_SHADER_FILE = "sdl/shader/simple_vs.glsl";
const char* FRAG_SHADER_FILE = "sdl/shader/simple_fs.glsl";
const int MAX_SHADER_BUFFER = 2048;

SHADER::SHADER(const char* vet_file , 
               const char* frag_file )
{
    vet_file_path = vet_file;
    frag_file_path = frag_file;
    shader_id = 0;
    init_shader();
}
SHADER::SHADER()
{
    vet_file_path = NULL;
    frag_file_path = NULL;
    shader_id = 0;
    init_shader();
}
void SHADER::read_shader_file(const char *fname,char *buf)
{
    int fp = open(fname,O_RDONLY);
    if(fp < 0)
    {
    	printf("read file error:%d,%s\n",errno,strerror(errno));
    }
    assert(fp > 0);

    int size = read(fp, buf, MAX_SHADER_BUFFER);
    buf[size] = '\0';
    printf("read shader file:%s success!\n",fname);
}
void SHADER::init_shader()
{
	
	if(shader_id) return;
    char shader_src_buf[MAX_SHADER_BUFFER];
    const char *buf = shader_src_buf;
    
    //vertex shader
    //"shader/vertextShader.glsl"
    if(!vet_file_path || defalut_vs_shader)
    {
    	buf = defalut_vs_shader;
    }
    else
    {
    	read_shader_file(vet_file_path,shader_src_buf);
    }
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&buf,NULL);
    glCompileShader(vertexShader);
    
    GLint success;
    GLchar infolog[512];
    glGetShaderiv(vertexShader,GL_COMPILE_STATUS,&success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader,512,NULL,infolog);
        std::cout<< "ERROR:SHADER::VERTEX::COMPILATION_FAILD\n"<<infolog<<std::endl;
        return;
        
    }
    //fragment shader
    //read_shader_file("shader/fragmentShader.glsl", shader_src_buf);
    if(!frag_file_path || defalut_fs_shader)
    {
    	buf = defalut_fs_shader;
    }
    else
    {
    	read_shader_file(frag_file_path, shader_src_buf);
    }
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,(const char* const*) &buf,NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader,GL_COMPILE_STATUS,&success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader,512,NULL,infolog);
        std::cout<< "ERROR:SHADER::FRAGMENT::COMPILATION_FAILD\n"<<infolog<<std::endl;
        return;
        
    }
    
    
    GLuint shaderProgram = glCreateProgram();
    
    glAttachShader(shaderProgram,vertexShader);
    glAttachShader(shaderProgram,fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram,GL_LINK_STATUS,&success);
    if(!success){
        glGetProgramInfoLog(shaderProgram,512,NULL,infolog);
        std::cout<< "ERROR:SHADER::PROGRAM::LINK_FAILD\n"<<infolog<<std::endl;
        return;
    }
    
    
    //glUseProgram(shaderProgram);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    shader_id = shaderProgram;
}
void SHADER::use()
{
    glUseProgram(shader_id);
}
GLint SHADER::glGetAttribLocation(const GLchar* name)
{
	assert(shader_id!=0);
    return ::glGetAttribLocation(shader_id, name);

}


VAOMGR::VAOMGR(int n)
{
    num = n;
}
void VAOMGR::gen_vao_vbo(GLfloat vertices[],size_t size,int pos_loc,int tex_loc)
{

    glGenVertexArrays(1,&vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,size, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(pos_loc, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat),(GLvoid *)0);
    glEnableVertexAttribArray(pos_loc);
    glVertexAttribPointer(tex_loc, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat),
                          (GLvoid *)(3*sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(tex_loc);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

}
void VAOMGR::bind()
{
    glBindVertexArray(vao);
}
void VAOMGR::ubind()
{
    glBindVertexArray(0);
}
void VAOMGR::draw()
{
    
    glDrawArrays(GL_TRIANGLES, 0, num);
}
VAOMGR::~VAOMGR()
{
    glDeleteVertexArrays(1,&vao);
    glDeleteBuffers(1,&vbo);
}

static GLfloat vertices[] = {
        // 第一个三角形       贴图坐标
        -1.0f,  1.0f, 0.0f, 0.0f,1.0f,   // 右上角
        1.0f,  -1.0f, 0.0f, 1.0f,0.0f,  // 右下角
        -1.0f, -1.0f, 0.0f, 0.0f,0.0f, // 左上角
        // 第二个三角形
        -1.0f,  1.0f, 0.0f, 0.0f,1.0f,  
        1.0f,   1.0f, 0.0f, 1.0f,1.0f,
        1.0f,  -1.0f, 0.0f, 1.0f,0.0f,
    };

Device::Device(int w, int h)
{
    win_width = w;
    win_height = h;
    // buff[0].alloc(w, h);
    // buff[1].alloc(w, h);
    // img_buf = NULL;
    cur_texture = NULL;
    position_loc = -1;
    tex_coord_loc = -1;

    wprintf(L"init Device!!\n");

    vao = new VAOMGR(6);
    // vao->gen_vao_vbo(vertices,sizeof(vertices));
    shader = new SHADER();
    // shader->init_shader();
    // shader.init_shader();
}
void Device::set_texture(GLTexture* tex)
{
	cur_texture = tex;
	// assert(!tex->is_bind);
	// if(cur_texture)
	// {
	// 	cur_texture->unbind();
	// 	cur_texture = tex;
	// 	cur_texture->bind();
	// }
	// else
	// {
	// 	cur_texture = tex;
	// 	cur_texture->bind();
	// }
	// printf("set cur_texture!\n");
}


void Device::update_texture()
{
	assert(cur_texture);
	cur_texture->update_texture();
}

void Device::init_render()
{
    shader->use();
    position_loc = shader->glGetAttribLocation("position");
    tex_coord_loc = shader->glGetAttribLocation("tex_coord");
    printf("pos_loc:%d,tex_loc:%d\n", position_loc,tex_coord_loc);

    vao->gen_vao_vbo(vertices,sizeof(vertices),position_loc,tex_coord_loc);
    vao->bind();
}
void Device::before_draw()
{
	if(cur_texture)
	{
		cur_texture->bind();
	}
}
void Device::after_draw()
{
	if(cur_texture)
	{
		cur_texture->unbind();
	}
    // Update screen
    if(gWindow)
    {
         SDL_GL_SwapWindow( gWindow );
//        eglSwapBuffers();
    }
}

void Device::render(unsigned int interval)
{
	before_draw();
	if(cur_texture)
		update_texture();
    vao->draw();
//    printf("render!!!!\n");
    
    after_draw();
}

void Device::update(unsigned int interval)
{
    render(interval);
}


void Device::release()
{
    // glBindTexture(GL_TEXTURE_2D, 0);
}

