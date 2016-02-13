// --------------------------------------------------------------------------
// OpenGL Program Class
// Copyright(C) 2007-2015
// Tamy Boubekeur
// --------------------------------------------------------------------------

#include "GLProgram.h"

#define printOpenGLError(X) printOglError ((X), __FILE__, __LINE__)
/// Returns 1 if an OpenGL error occurred, 0 otherwise.
        static int printOglError (const std::string & msg, const char * file, int line) {
            GLenum glErr;
            int    retCode = 0;
            glErr = glGetError ();
            while (glErr != GL_NO_ERROR) {
                printf ("glError in file %s @ line %d: %s - %s\n", file, line, gluErrorString(glErr), msg.c_str ());
                retCode = 1;
                glErr = glGetError ();
            }
            return retCode;
        }

        void checkGLExceptions () {
                GLenum glerr = glGetError ();
                switch (glerr) {
                case GL_INVALID_ENUM:
                    throw Exception ("Invalid Enum");
                    break;
                case GL_INVALID_VALUE:
                    throw Exception ("Invalid Value");
                    break;
                case GL_INVALID_OPERATION:
                    throw Exception ("Invalid Operation");
                    break;

                case GL_STACK_OVERFLOW:
                    throw Exception ("Stack Overflow");
                    break;
                case GL_STACK_UNDERFLOW:
                    throw Exception ("Stack Underflow");
                    break;
                case GL_OUT_OF_MEMORY:
                    throw Exception ("Out of Memory");
                    break;
                case GL_TABLE_TOO_LARGE:
                    throw Exception ("Table Too Large");
                    break;
                case GL_NO_ERROR:
                    break;
                default:
                    throw Exception ("Other Error");
                    break;
                }
            }

        Shader::Shader (const std::string & name, GLuint type) {
            _id = glCreateShader (type);
            _name = name;
            _type = type;
            _filename = "";
            _source = "";
        }

        Shader::~Shader () {
            if (_id != 0)
                glDeleteShader (_id);
        }

        void Shader::setSource (const std::string & source) {
            _source = source;
        }

        void Shader::compile () {
            const GLchar * tmp = _source.c_str();
            glShaderSource (_id, 1, &tmp, NULL);
            glCompileShader (_id);
            printOpenGLError ("Compiling Shader " + name ());  // Check for OpenGL errors
            GLint shaderCompiled;
            glGetShaderiv (_id, GL_COMPILE_STATUS, &shaderCompiled);
            printOpenGLError ("Compiling Shader " + name ());  // Check for OpenGL errors
            if (!shaderCompiled)
                throw Exception ("Error: shader not compiled. Info. Log.:\n" + infoLog () + "\nSource:\n" + _source);
        }

        void Shader::loadFromFile (const std::string & filename) {
            _filename = filename;
            std::ifstream in (_filename.c_str ());
            if (!in)
                throw Exception ("Error loading shader source file: " + _filename);
            std::string source;
            char c[2];
            c[1]='\0';
            while (in.get (c[0]))
                source.append (c);
            in.close ();
            setSource (source);
        }

        void Shader::reload () {
            if (_filename != "") {
                loadFromFile (std::string (_filename));
                compile ();
            }
        }

        std::string Shader::infoLog () {
            std::string infoLogStr = "";
            int infologLength = 0;
            glGetShaderiv (_id, GL_INFO_LOG_LENGTH, &infologLength);
            printOpenGLError ("Gathering Shader InfoLog Length for " + name ());
            if (infologLength > 0) {
                GLchar *str = new GLchar[infologLength];
                int charsWritten  = 0;
                glGetShaderInfoLog (_id, infologLength, &charsWritten, str);
                printOpenGLError ("Gathering Shader InfoLog for " + name ());
                infoLogStr  = std::string (str);
                delete [] str;
            }
            return infoLogStr;
        }

        Program::Program (const std::string & name) {
			_id = glCreateProgram();
			_name = name;
		}

        Program::~Program () {
            glDeleteProgram (_id);
        }

        void Program::attach (Shader * shader) {
            glAttachShader (_id, shader->id());
            _shaders.push_back (shader);
        }

        void Program::detach (Shader * shader) {
            for (unsigned int i = 0; i < _shaders.size (); i++)
                if (_shaders[i]->id () == shader->id ())
                    glDetachShader (_id, shader->id());
        }

        void Program::link () {
            glLinkProgram (_id);
            printOpenGLError ("Linking Program " + name ());
            GLint linked;
            glGetProgramiv (_id, GL_LINK_STATUS, &linked);
            if (!linked)
                throw Exception ("Shaders not linked: " + infoLog ());
        }

        void Program::use () {
            glUseProgram (_id);
        }

        void Program::stop () {
            glUseProgram (0);
        }

        std::string Program::infoLog () {
            std::string infoLogStr = "";
            int infologLength = 0;
            glGetProgramiv (_id, GL_INFO_LOG_LENGTH, &infologLength);
            printOpenGLError ("Gathering Shader InfoLog for Program " + name ());
            if (infologLength > 0) {
                GLchar *str = new GLchar[infologLength];
                int charsWritten  = 0;
                glGetProgramInfoLog (_id, infologLength, &charsWritten, str);
                printOpenGLError ("Gathering Shader InfoLog for Program " + name ());
                infoLogStr  = std::string (str);
                delete [] str;
            }
            return infoLogStr;
        }

        GLint Program::getUniformLocation (const std::string & uniformName) {
            const GLchar * cname = uniformName.c_str ();
            GLint loc = glGetUniformLocation (_id, cname);
            if (loc == -1)
                throw Exception (std::string ("Program Error: No such uniform named ") + uniformName);
            printOpenGLError ("Wrong Uniform Variable [" + uniformName + "] for Program [" + name () + "]");
            return loc;
        }

        void Program::setUniform1f (GLint location, float value) {
            use ();
            glUniform1f (location, value);
        }

        void Program::setUniform1f (const std::string & name, float value) {
            use ();
            glUniform1f (getUniformLocation (name), value);
        }

        void Program::setUniform2f (GLint location, float value0, float value1) {
            use ();
            glUniform2f (location, value0, value1);
        }

        void Program::setUniform2f (const std::string & name, float value0, float value1) {
            use ();
            glUniform2f (getUniformLocation (name), value0, value1);
        }

        void Program::setUniform3f (GLint location, float value0, float value1, float value2) {
            use ();
            glUniform3f (location, value0, value1, value2);
        }

        void Program::setUniform3f (const std::string & name, float value0, float value1, float value2) {
            use ();
            glUniform3f (getUniformLocation (name), value0, value1, value2);
        }

        void Program::setUniform4f (GLint location, float value0, float value1, float value2, float value3) {
            use ();
            glUniform4f (location, value0, value1, value2, value3);
        }

        void Program::setUniform4f (const std::string & name, float value0, float value1, float value2, float value3) {
            use ();
            glUniform4f (getUniformLocation (name), value0, value1, value2, value3);
        }

        void Program::setUniformMatrix4fv (GLint location, const float * values) {
            use ();
            glUniformMatrix4fv (location, 1, GL_FALSE, values);
        }

        void Program::setUniformMatrix4fv (const std::string & name, const float * values) {
            use ();
            setUniformMatrix4fv (getUniformLocation (name), values);
        }

        void Program::setUniformNf (GLint location, unsigned int numValues, const float * values) {
            use ();
            switch (numValues) {
            case 1: glUniform1f (location, values[0]); break;
            case 2: glUniform2f (location, values[0], values[1]); break;
            case 3: glUniform3f (location, values[0], values[1], values[2]); break;
            case 4: glUniform4f (location, values[0], values[1], values[2], values[3]); break;
            default: throw Exception ("Program Error: Wrong number of values to set for uniform float array."); break;
            }
        }

        void Program::setUniformNf (const std::string & name, unsigned int numValues, const float * values) {
            use ();
            GLint loc = getUniformLocation (name);
            switch (numValues) {
            case 1: glUniform1f (loc, values[0]); break;
            case 2: glUniform2f (loc, values[0], values[1]); break;
            case 3: glUniform3f (loc, values[0], values[1], values[2]); break;
            case 4: glUniform4f (loc, values[0], values[1], values[2], values[3]); break;
            default: throw Exception ("Wrong number of values to set for uniform float array " + name + "."); break;
            }
        }

        void Program::setUniform1i (GLint location, int value) {
            use ();
            glUniform1i (location, value);
        }

        void Program::setUniform1i (const std::string & name, int value) {
            use ();
            glUniform1i (getUniformLocation (name), value);
        }

        void Program::setUniformNi (GLint location, unsigned int numValues, const int * values) {
            use ();
            switch (numValues) {
            case 1: glUniform1i (location, values[0]); break;
            case 2: glUniform2i (location, values[0], values[1]); break;
            case 3: glUniform3i (location, values[0], values[1], values[2]); break;
            case 4: glUniform4i (location, values[0], values[1], values[2], values[3]); break;
            default: throw Exception ("Program Error: Wrong number of values to set for uniform int array."); break;
            }
        }

        void Program::setUniformNi (const std::string & name, unsigned int numValues, const int * values) {
            use ();
            GLint loc = getUniformLocation (name);
            switch (numValues) {
            case 1: glUniform1i (loc, values[0]); break;
            case 2: glUniform2i (loc, values[0], values[1]); break;
            case 3: glUniform3i (loc, values[0], values[1], values[2]); break;
            case 4: glUniform4i (loc, values[0], values[1], values[2], values[3]); break;
            default: throw Exception ("Program Error: Wrong number of values to set for uniform int array " + name + "."); break;
            }
        }

        void Program::reload () {
            for (unsigned int i = 0; i < _shaders.size (); i++) {
                _shaders[i]->reload ();
                attach (_shaders[i]);
            }
            link ();
        }

        Program * Program::genVFProgram (const std::string & name,
                                         const std::string & vertexShaderFilename,
                                         const std::string & fragmentShaderFilename) {
			Program * p = new Program (name);
            Shader * vs = new Shader (name + " Vertex Shader", GL_VERTEX_SHADER);
            Shader * fs = new Shader (name + " Fragment Shader",GL_FRAGMENT_SHADER);
            vs->loadFromFile (vertexShaderFilename);
			std::cout << "vertex shader: [" + vertexShaderFilename + "] loaded successfully" << std::endl;
            vs->compile ();
            p->attach(vs);
			std::cout << "vertex shader compiled successfully" << std::endl;
            fs->loadFromFile (fragmentShaderFilename);
			std::cout << "fragment shader: [" + fragmentShaderFilename + "] loaded successfully" << std::endl;
            fs->compile ();
            p->attach(fs);
			std::cout << "fragment shader compiled successfully" << std::endl;
            p->link();
			std::cout << "linked successfully" << std::endl;
            //p->use ();
            return p;
        }
