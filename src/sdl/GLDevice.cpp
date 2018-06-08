
#include "GLDevice.hpp"
#include "shaders.hpp"

extern SDL_Window* gWindow;
extern const char* simple_vs_shader;
extern const char* simple_fs_shader;

const char* VERTEX_SHADER_FILE = "sdl/shader/simple_vs.glsl";
const char* FRAG_SHADER_FILE = "sdl/shader/simple_fs.glsl";
const int MAX_SHADER_BUFFER = 2048;

SHADER::SHADER(const char* vet_file , 
               const char* frag_file )
{
    vet_file_path = vet_file;
    frag_file_path = frag_file;
    shader = 0;
    init_shader();
}
SHADER::SHADER()
{
    vet_file_path = NULL;
    frag_file_path = NULL;
    shader = 0;
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
	if(shader) return;
    char shader_src_buf[MAX_SHADER_BUFFER];
    const char *buf = shader_src_buf;
    
    //vertex shader
    //"shader/vertextShader.glsl"
    if(!vet_file_path || simple_vs_shader)
    {
    	buf = simple_vs_shader;
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
    if(!frag_file_path || simple_fs_shader)
    {
    	buf = simple_fs_shader;
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
    
    shader = shaderProgram;
}
void SHADER::use()
{
    glUseProgram(shader);
}


VAOMGR::VAOMGR(int n)
{
    num = n;
}
void VAOMGR::gen_vao_vbo(GLfloat vertices[],size_t size)
{

    glGenVertexArrays(1,&vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,size, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat),(GLvoid *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat),
                          (GLvoid *)(3*sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);

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

    wprintf(L"init Device!!\n");

    vao = new VAOMGR(6);
    vao->gen_vao_vbo(vertices,sizeof(vertices));
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
    	SDL_GL_SwapWindow( gWindow );
}

void Device::render(unsigned int interval)
{
	before_draw();
	if(cur_texture)
		update_texture();
    vao->draw();
    
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

