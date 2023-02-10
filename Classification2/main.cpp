#include <iostream>
#include <SFML/Window.hpp>
#include <glad/glad.h>
#include "NeuralNetwork.hpp"
#include <Windows.h>
#include <fstream>
using namespace std;
using namespace sf;



GLuint CreateShader(string FileName, char Type)
{

    static int len = 8000;
    fstream file;
    if (Type == 'f')
    {
        file.open(FileName + ".frag");
    }
    else if (Type == 'v')
    {
        file.open(FileName + ".vert");
    }
    else if (Type == 'g')
    {
        file.open(FileName + ".geom");
    }
    char* txt = new char[len];
    file.getline(txt, len, '\0');
    //cout << text << endl;
    file.close();
    GLuint shader;
    if (Type == 'f')
    {
        shader = glCreateShader(GL_FRAGMENT_SHADER);
    }
    else if (Type == 'v')
    {
        shader = glCreateShader(GL_VERTEX_SHADER);
    }
    else if (Type == 'g')
    {
        shader = glCreateShader(GL_GEOMETRY_SHADER);
    }
    glShaderSource(shader, 1, &txt, NULL);
    glCompileShader(shader);
    delete txt;
    GLint ok;
    GLchar log[2000];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        glGetShaderInfoLog(shader, 2000, NULL, log);
        printf("%s\n", log);
    }
    return shader;

}

void LoadShader(string FileName, GLuint& shader)
{
    GLuint  FragShader = CreateShader(FileName, 'f');
    //GLuint  VertShader = CreateShader(FileName, 'v');
    //GLuint  GeomShader = CreateShader(FileName, 'g');

    shader = glCreateProgram();
    glAttachShader(shader, FragShader);
    //glAttachShader(shader, VertShader);
    //glAttachShader(shader, GeomShader);
    glLinkProgram(shader);

}

void Init()
{
    gladLoadGL();
}

struct FrameBuffer
{
    unsigned int texture;
    unsigned char* data;
    int FWidth;
    int FHeight;


    void LoadFrame()
    {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FWidth, FHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }


    unsigned char &operator()(int i,int j,int k)
    {
        return data[j * FWidth * 4 + i * 4 + k];
    }

    void clear(int r, int g, int b, int a)
    {
        for (int i = 0; i < FWidth; i++)
        {
            for (int j = 0; j < FHeight; j++)
            {
                this->operator()(i, j, 0) = static_cast <char>(r);
                this->operator()(i, j, 1) = static_cast <char>(g);
                this->operator()(i, j, 2) = static_cast <char>(b);
                this->operator()(i, j, 3) = static_cast <char>(a);
            }
        }
    }

    FrameBuffer(int FWidth, int FHeight)
    {
        this->FWidth = FWidth;
        this->FHeight = FHeight;
        data = new unsigned char[FWidth * FHeight * 4];
    }

};


struct circle
{
    int x;
    int y;
    int rad;
    int r;
    int g;
    int b;
    int a;

    void draw(FrameBuffer& frame)
    {
        int minx = x - rad;
        int maxx = x + rad;
        int miny = y - rad;
        int maxy = y + rad;
        for (int i = minx; i <= maxx; i++)
        {
            for (int j = miny; j <= maxy; j++)
            {
                double range = sqrt(pow(x - i, 2) + pow(y - j, 2)) - 0.2;
                if (range <= rad)
                {
                    frame(i, j, 0) = r;
                    frame(i, j, 1) = g;
                    frame(i, j, 2) = b;
                    frame(i, j, 3) = a;
                }
            }
        }
    }

    void setPosition(int x, int y)
    {
        this->x = x;
        this->y = y;
    }
    void setSize(int rad)
    {
        this->rad = rad;
    }
    void setColor(int r, int g, int b, int a)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }
    void setParams(int x, int y, int rad, int r, int g, int b, int a)
    {
        setPosition(x, y);
        setSize(rad);
        setColor(r,g,b,a);
    }


    circle(int x, int y, int rad, int r, int g, int b, int a)
    {
        setParams(x, y, rad, r, g, b, a);
    }
    circle()
    {

    }

};







int main()
{
    srand(time(NULL));

    ContextSettings settings;
    settings.depthBits = 32;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 0;
    settings.majorVersion = 3.3;
    settings.minorVersion = 3.3;
    settings.sRgbCapable = 0;
    //int width = GetSystemMetrics(SM_CXSCREEN);
    //int heigh = GetSystemMetrics(SM_CYSCREEN);
    int width = 800;
    int heigh = 600;
    float k = float(width) / float(heigh);
    Window window(VideoMode(width, heigh), "OpenGL", Style::Default, settings);
    //window.setVerticalSyncEnabled(true);
    //window.setActive(true);
    //ShowCursor(1);
    Init();

    bool running = true;
    GLuint shader;
    LoadShader("sh", shader);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, heigh, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);

   
   
    FrameBuffer frame(width, heigh);
    frame.clear(0, 0, 255, 255);
    bool lbuttonunpressed = true;
    bool rbuttonunpressed = true;

    int sizeobj1 = 0;
    int sizeobj2 = 0;
    circle* c1 = new circle[1000];
    circle* c2 = new circle[1000];


    Network nn;
    //nn.useBias = false;
    nn.setArch(4, 2, 4,5, 2);
    nn.setRandWeights(0, 1, 1);

    //bool JunPressed = true;
   
    vec in(2);
    vec out(2);
    vec outright(2);


    Clock clock;
    float time;
    while (running)
    {
        time = clock.getElapsedTime().asSeconds();


        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {

                running = false;
            }
            else if (event.type == sf::Event::Resized)
            {

                glViewport(0, 0, event.size.width, event.size.height);
            }
            if (Keyboard::isKeyPressed(Keyboard::Escape))
            {
                running = false;
                window.close();
            }
        }
        
        if (Mouse::isButtonPressed(Mouse::Left) && lbuttonunpressed)
        {
            c1[sizeobj1].setParams(Mouse::getPosition(window).x, heigh-Mouse::getPosition(window).y, 3, 255, 0, 0, 255);
            sizeobj1++;
            lbuttonunpressed = false;
        }
        else if(!Mouse::isButtonPressed(Mouse::Left))
        {
            lbuttonunpressed = true;
        }

        if (Mouse::isButtonPressed(Mouse::Right) && rbuttonunpressed)
        {
            c2[sizeobj2].setParams(Mouse::getPosition(window).x, heigh - Mouse::getPosition(window).y, 3, 255, 255, 0, 255);
            sizeobj2++;
            rbuttonunpressed = false;
        }
        else if (!Mouse::isButtonPressed(Mouse::Right))
        {
            rbuttonunpressed = true;
        }


        bool huita = true;
        int c1huita = 0;
        int c2huita = 0;
        while (huita)
        {
            if (c1huita < sizeobj1)
            {
                
                in[0] = (double)c1[c1huita].x/width;
                in[1] = (double)c1[c1huita].y/heigh;
                
                outright[0] = 1;
                outright[1] = 0;
                //cout << outright[0] << " " << outright[1] << endl;
                nn.setInputLayer(in);
                nn.work();
                nn.setError(outright);
                nn.updateWeights(1);
                //cout << "c1" << endl;
                c1huita++;

                //nn.work();
                //vec out = nn.getOutputLayer();
                //cout << out[0] << " " << out[1] << endl;

            }
            if (c2huita < sizeobj2)
            {
                
                in[0] = (double)c2[c2huita].x/ width;
                in[1] = (double)c2[c2huita].y/ heigh;
               
                outright[0] = 0;
                outright[1] = 1;

                nn.setInputLayer(in);
                nn.work();
                nn.setError(outright);
                nn.updateWeights(1);
                //cout << "c1" << endl;
                c2huita++;

                //nn.work();
                //vec out = nn.getOutputLayer();
                //cout << out[0] << " " << out[1] << endl;

            }

            if (c1huita == sizeobj1 && c2huita == sizeobj2)
            {
                huita = false;
            }
        }


        if (time >= 1)
        {
            clock.restart();
            //cout << "Show" << endl;
            for (int i = 0; i < frame.FWidth; i++)
            {
                for (int j = 0; j < frame.FHeight; j++)
                {
      
                    in[0] = (double)i / width;
                    in[1] = (double)j / heigh;
                    nn.setInputLayer(in);
                    nn.work();
                    //out = nn.getOutputLayer();
                    out.copyData(nn.l[nn.size - 1]);
                   
                    int colorG = out[0] * 255;
                    int colorB = out[1] * 255;
                    
                    frame(i, j, 0) = static_cast <char>(0);
                    frame(i, j, 1) = static_cast <char>(colorG);
                    frame(i, j, 2) = static_cast <char>(colorB);
                    frame(i, j, 3) = static_cast <char>(255);

                }

            }
            //cout << "Show end" << endl;
            //JunPressed = false;
        }
        

        //cout << "a" << endl;
        //frame.clear(0, 0, 255, 255);


        //cout << Mouse::getPosition(window).x << " " << Mouse::getPosition(window).y << endl;

        //nn.saveToFile("nn.nn");



        for (int i = 0; i < sizeobj1; i++)
        {
            c1[i].draw(frame);
        }
        for (int i = 0; i < sizeobj2; i++)
        {
            c2[i].draw(frame);
        }
        frame.LoadFrame();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, frame.texture);
        glUniform1i(glGetUniformLocation(shader, "texture"), 0);
        
        glUseProgram(shader);
        glPushMatrix();
        glColor3f(1, 0, 0);
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(width, 0);
        glVertex2f(width, heigh);
        glVertex2f(0, heigh);
        glEnd();
        glPopMatrix();


        window.display();
    }

    return 0;
}