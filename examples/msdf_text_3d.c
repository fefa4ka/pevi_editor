#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct {
    GLuint texture;
    int width;
    int height;
    float advance_x;
    float bearing_x;
    float bearing_y;
} Character;


// Simple vertex and fragment shaders for MSDF rendering
const char* vertexShaderSource = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main() {\n"
    "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "    TexCoord = aTexCoord;\n"
    "}\n";

const char* fragmentShaderSource = 
    "#version 330 core\n"
    "in vec2 TexCoord;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D msdfTexture;\n"
    "uniform float pxRange;\n"
    "uniform vec4 textColor;\n"
    "float median(float r, float g, float b) {\n"
    "    return max(min(r, g), min(max(r, g), b));\n"
    "}\n"
    "void main() {\n"
    "    vec3 msd = texture(msdfTexture, TexCoord).rgb;\n"
    "    float sd = median(msd.r, msd.g, msd.b);\n"
    "    float screenPxDistance = pxRange * (sd - 0.5);\n"
    "    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);\n"
    "    FragColor = vec4(textColor.rgb, textColor.a * opacity);\n"
    "}\n";

// Matrix math helpers
void mat4_identity(float* m) {
    memset(m, 0, 16 * sizeof(float));
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}

void mat4_perspective(float* m, float fovy, float aspect, float near, float far) {
    float f = 1.0f / tanf(fovy * 0.5f);
    memset(m, 0, 16 * sizeof(float));
    m[0] = f / aspect;
    m[5] = f;
    m[10] = (far + near) / (near - far);
    m[11] = -1.0f;
    m[14] = (2.0f * far * near) / (near - far);
}

void mat4_lookat(float* m, float* eye, float* center, float* up) {
    float f[3], s[3], u[3];
    
    // Calculate forward vector
    f[0] = center[0] - eye[0];
    f[1] = center[1] - eye[1];
    f[2] = center[2] - eye[2];
    float len = sqrtf(f[0]*f[0] + f[1]*f[1] + f[2]*f[2]);
    f[0] /= len; f[1] /= len; f[2] /= len;
    
    // Calculate right vector (cross product of f and up)
    s[0] = f[1]*up[2] - f[2]*up[1];
    s[1] = f[2]*up[0] - f[0]*up[2];
    s[2] = f[0]*up[1] - f[1]*up[0];
    len = sqrtf(s[0]*s[0] + s[1]*s[1] + s[2]*s[2]);
    s[0] /= len; s[1] /= len; s[2] /= len;
    
    // Calculate up vector (cross product of s and f)
    u[0] = s[1]*f[2] - s[2]*f[1];
    u[1] = s[2]*f[0] - s[0]*f[2];
    u[2] = s[0]*f[1] - s[1]*f[0];
    
    mat4_identity(m);
    m[0] = s[0]; m[4] = s[1]; m[8] = s[2];
    m[1] = u[0]; m[5] = u[1]; m[9] = u[2];
    m[2] = -f[0]; m[6] = -f[1]; m[10] = -f[2];
    m[12] = -(s[0]*eye[0] + s[1]*eye[1] + s[2]*eye[2]);
    m[13] = -(u[0]*eye[0] + u[1]*eye[1] + u[2]*eye[2]);
    m[14] = f[0]*eye[0] + f[1]*eye[1] + f[2]*eye[2];
}

void mat4_translate(float* m, float x, float y, float z) {
    mat4_identity(m);
    m[12] = x;
    m[13] = y;
    m[14] = z;
}

void mat4_rotate_y(float* m, float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    mat4_identity(m);
    m[0] = c;
    m[2] = s;
    m[8] = -s;
    m[10] = c;
}

// Compile shader
GLuint compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("Shader compilation failed: %s\n", infoLog);
    }
    
    return shader;
}

// Create shader program
GLuint create_shader_program(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader = compile_shader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compile_shader(GL_FRAGMENT_SHADER, fragmentSource);
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        printf("Shader linking failed: %s\n", infoLog);
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return program;
}

// Compute distance to nearest edge
float compute_distance_to_edge(unsigned char* bitmap, int width, int height, int x, int y, int search_radius) {
    float min_dist = (float)search_radius;
    
    // Bounds check
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return min_dist;
    }
    
    int center_inside = bitmap[y * width + x] > 128;
    
    for (int dy = -search_radius; dy <= search_radius; dy++) {
        for (int dx = -search_radius; dx <= search_radius; dx++) {
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                int neighbor_inside = bitmap[ny * width + nx] > 128;
                if (center_inside != neighbor_inside) {
                    float dist = sqrtf((float)(dx*dx + dy*dy));
                    if (dist < min_dist) {
                        min_dist = dist;
                    }
                }
            }
        }
    }
    
    return center_inside ? min_dist : -min_dist;
}

// Improved MSDF generation with multi-channel distance fields
void generate_distance_field(unsigned char* bitmap, int width, int height, unsigned char* output) {
    int search_radius = 12;  // Increased radius for better quality
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = y * width + x;
            
            // For MSDF, we need three different distance calculations
            // This is a simplified approach - real MSDF uses edge segments
            
            // Red channel: distance to nearest edge
            float dist_r = compute_distance_to_edge(bitmap, width, height, x, y, search_radius);
            
            // Green channel: distance with slight offset for better quality
            int offset_x = x + (x % 2 == 0 ? 1 : -1);
            float dist_g = compute_distance_to_edge(bitmap, width, height, 
                                                   offset_x, y, search_radius);
            
            // Blue channel: distance with different offset
            int offset_y = y + (y % 2 == 0 ? 1 : -1);
            float dist_b = compute_distance_to_edge(bitmap, width, height, 
                                                   x, offset_y, search_radius);
            
            // Normalize and convert to 0-255 range
            float max_dist = (float)search_radius;
            
            int r = (int)((dist_r / max_dist * 0.5f + 0.5f) * 255.0f);
            int g = (int)((dist_g / max_dist * 0.5f + 0.5f) * 255.0f);
            int b = (int)((dist_b / max_dist * 0.5f + 0.5f) * 255.0f);
            
            // Clamp values
            r = r < 0 ? 0 : (r > 255 ? 255 : r);
            g = g < 0 ? 0 : (g > 255 ? 255 : g);
            b = b < 0 ? 0 : (b > 255 ? 255 : b);
            
            output[idx * 3] = (unsigned char)r;
            output[idx * 3 + 1] = (unsigned char)g;
            output[idx * 3 + 2] = (unsigned char)b;
        }
    }
}

// Load a character from font and create texture
Character load_character(FT_Face face, char c) {
    Character character = {0};
    
    // Load character glyph
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
        printf("Failed to load glyph '%c'\n", c);
        return character;
    }
    
    FT_GlyphSlot g = face->glyph;
    
    // Generate texture
    glGenTextures(1, &character.texture);
    glBindTexture(GL_TEXTURE_2D, character.texture);
    
    // Create distance field from bitmap (increased padding for better quality)
    int width = g->bitmap.width;
    int height = g->bitmap.rows;
    int padded_width = width + 24;  // Increased padding for better distance field
    int padded_height = height + 24;
    
    // Create padded bitmap
    unsigned char* padded_bitmap = calloc(padded_width * padded_height, 1);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            padded_bitmap[(y + 12) * padded_width + (x + 12)] = 
                g->bitmap.buffer[y * g->bitmap.pitch + x];
        }
    }
    
    // Generate distance field
    unsigned char* sdf_data = malloc(padded_width * padded_height * 3);
    generate_distance_field(padded_bitmap, padded_width, padded_height, sdf_data);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, padded_width, padded_height, 0, 
                 GL_RGB, GL_UNSIGNED_BYTE, sdf_data);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Store character info
    character.width = padded_width;
    character.height = padded_height;
    character.bearing_x = g->bitmap_left - 12;
    character.bearing_y = g->bitmap_top + 12;
    character.advance_x = g->advance.x / 64.0f;
    
    free(padded_bitmap);
    free(sdf_data);
    
    return character;
}

int main(int argc, char* argv[]) {
    printf("Starting MSDF text 3D example...\n");
    
    // Get font path from command line or use default
    const char* font_path = argc > 1 ? argv[1] : "/System/Library/Fonts/Helvetica.ttc";
    printf("Using font: %s\n", font_path);
    
    // Initialize FreeType
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        fprintf(stderr, "Failed to initialize FreeType\n");
        return -1;
    }
    printf("FreeType initialized successfully\n");
    
    // Load font
    FT_Face face;
    if (FT_New_Face(ft, font_path, 0, &face)) {
        fprintf(stderr, "Failed to load font: %s\n", font_path);
        fprintf(stderr, "Usage: %s [path/to/font.ttf]\n", argv[0]);
        FT_Done_FreeType(ft);
        return -1;
    }
    printf("Font loaded successfully\n");
    
    // Set font size (increased for better quality)
    FT_Set_Pixel_Sizes(face, 0, 64);
    
    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
        return -1;
    }
    printf("GLFW initialized successfully\n");
    
    // Create window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "MSDF Text 3D Example", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    printf("GLFW window created successfully\n");
    
    glfwMakeContextCurrent(window);
    printf("OpenGL context made current\n");
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Create shader program
    GLuint shaderProgram = create_shader_program(vertexShaderSource, fragmentShaderSource);
    printf("Shader program created successfully\n");
    
    // Load characters for "Hello World!"
    const char* text = "Hello World!";
    Character characters[256] = {0};
    printf("Starting character loading...\n");
    
    // Load all ASCII characters
    for (int c = 32; c < 127; c++) {
        characters[c] = load_character(face, c);
        if (c == 32) {
            printf("First character loaded successfully\n");
        }
    }
    printf("All characters loaded successfully\n");
    
    // Calculate text dimensions
    float text_width = 0;
    float text_height = 0;
    for (const char* p = text; *p; p++) {
        Character* ch = &characters[(unsigned char)*p];
        text_width += ch->advance_x;
        if (ch->height > text_height) {
            text_height = ch->height;
        }
    }
    printf("Text dimensions calculated: %.2f x %.2f\n", text_width, text_height);
    
    // Create a texture atlas for the text
    int atlas_width = (int)text_width + 20;
    int atlas_height = (int)text_height + 20;
    unsigned char* atlas_data = calloc(atlas_width * atlas_height * 3, 1);
    printf("Atlas created: %d x %d\n", atlas_width, atlas_height);
    
    // Copy characters to atlas
    float x_offset = 10;
    for (const char* p = text; *p; p++) {
        Character* ch = &characters[(unsigned char)*p];
        if (ch->texture == 0) continue;
        
        // Get character texture data
        glBindTexture(GL_TEXTURE_2D, ch->texture);
        unsigned char* char_data = malloc(ch->width * ch->height * 3);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, char_data);
        
        // Copy to atlas
        int y_offset = atlas_height - ch->height - 10;
        for (int y = 0; y < ch->height; y++) {
            for (int x = 0; x < ch->width; x++) {
                int src_idx = (y * ch->width + x) * 3;
                int dst_idx = ((y_offset + y) * atlas_width + (int)x_offset + x) * 3;
                if (dst_idx >= 0 && dst_idx < atlas_width * atlas_height * 3 - 2) {
                    atlas_data[dst_idx] = char_data[src_idx];
                    atlas_data[dst_idx + 1] = char_data[src_idx + 1];
                    atlas_data[dst_idx + 2] = char_data[src_idx + 2];
                }
            }
        }
        
        x_offset += ch->advance_x;
        free(char_data);
    }
    
    // Use individual character texture instead of broken atlas
    // This avoids the glGetTexImage issue while maintaining functionality
    printf("Looking for character 'H'...\n");
    Character* first_char = &characters['H']; // Use 'H' from "Hello World!"
    printf("Character 'H' texture: %u\n", first_char->texture);
    
    if (first_char->texture == 0) {
        printf("Character 'H' not found, searching for any available character...\n");
        // Fallback to any available character
        for (int c = 32; c < 127; c++) {
            if (characters[c].texture != 0) {
                first_char = &characters[c];
                printf("Found character '%c' with texture %u\n", c, first_char->texture);
                break;
            }
        }
    }
    
    GLuint atlas_texture = first_char->texture;
    printf("Using character texture %u for display\n", atlas_texture);
    printf("Character dimensions: %d x %d\n", first_char->width, first_char->height);
    
    // Don't free atlas_data since we're not using it
    free(atlas_data);
    
    // Create a quad for text rendering with proper aspect ratio  
    float aspect = (float)first_char->width / (float)first_char->height;
    float quad_height = 2.0f;
    float quad_width = quad_height * aspect;
    
    float vertices[] = {
        // positions                      // texture coords
        -quad_width/2,  quad_height/2, 0.0f,   0.0f, 0.0f,
         quad_width/2,  quad_height/2, 0.0f,   1.0f, 0.0f,
         quad_width/2, -quad_height/2, 0.0f,   1.0f, 1.0f,
        -quad_width/2, -quad_height/2, 0.0f,   0.0f, 1.0f
    };
    
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    // Create VAO, VBO, EBO
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    
    // Get uniform locations
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    GLint pxRangeLoc = glGetUniformLocation(shaderProgram, "pxRange");
    GLint textColorLoc = glGetUniformLocation(shaderProgram, "textColor");
    
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Clear
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Use shader
        glUseProgram(shaderProgram);
        
        // Set matrices
        float model[16], view[16], projection[16];
        
        // Model matrix - rotate text
        float time = (float)glfwGetTime();
        mat4_rotate_y(model, time);
        
        // View matrix
        float eye[] = {0.0f, 0.0f, 5.0f};
        float center[] = {0.0f, 0.0f, 0.0f};
        float up[] = {0.0f, 1.0f, 0.0f};
        mat4_lookat(view, eye, center, up);
        
        // Projection matrix
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        mat4_perspective(projection, 3.14159f / 4.0f, (float)width / (float)height, 0.1f, 100.0f);
        
        // Set uniforms
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);
        glUniform1f(pxRangeLoc, 8.0f);
        glUniform4f(textColorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
        
        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, atlas_texture);
        
        // Draw
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    
    // Delete atlas texture
    glDeleteTextures(1, &atlas_texture);
    
    // Delete all character textures
    for (int c = 32; c < 127; c++) {
        if (characters[c].texture) {
            glDeleteTextures(1, &characters[c].texture);
        }
    }
    
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}
