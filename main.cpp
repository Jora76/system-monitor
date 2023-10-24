#include "src/header.h"
#include "imgui_stdlib.h"
#include <SDL2/SDL.h>
#include <cstdio>
#include <chrono>
#include <thread>

// --------------   GLOBAL   ----------------

char *hostname;
const char *osName;
const char *user;
chrono::steady_clock::time_point display_start = chrono::steady_clock::now();
int system_FPSCounter = 30;
float system_graphScale = 50;
bool animate = true;
float totalSpace;
float freeSpace;

// --------------   CPU   ----------------

chrono::steady_clock::time_point CPU_start = chrono::steady_clock::now();
vector<float> CPUState;
float CPUPercent;
string cpu;

// --------------   FAN   ----------------

chrono::steady_clock::time_point fan_start = chrono::steady_clock::now();
float system_graphFanScale = 5500.f;
vector<float> fanState;
string fan_level;
float fanRPM;

// --------------   THERMAL   ----------------

chrono::steady_clock::time_point therm_start = chrono::steady_clock::now();
vector<float> thermState;
float thermal_state;

// --------------   MEM/SWAP   ----------------

float memTotal;
float memState;
float swapTotal;
float swapState;
vector<Process> tabProcess;

// --------------   NETWORK    ----------------

array<string, 7> tabDay{"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
string tabMonth[12]{"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
Networks ip_addr;

/*
NOTE : You are free to change the code as you wish, the main objective is to make the
       application work and pass the audit.

       It will be provided the main function with the following functions :

       - `void systemWindow(const char *id, ImVec2 size, ImVec2 position)`
            This function will draw the system window on your screen
       - `void memoryProcessesWindow(const char *id, ImVec2 size, ImVec2 position)`
            This function will draw the memory and processes window on your screen
       - `void networkWindow(const char *id, ImVec2 size, ImVec2 position)`
            This function will draw the network window on your screen
*/

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h> // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h> // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h> // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <glad/gl.h> // Initialize with gladLoadGL(...) or gladLoaderLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE      // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h> // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE        // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h> // Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// systemWindow, display information for the system monitorization
void systemWindow(const char *id, ImVec2 size, ImVec2 position)
{
    // Initialisation de la fenêtre
    ImGui::Begin(id);
    ImGui::SetWindowSize(id, size);
    ImGui::SetWindowPos(id, position);

    // Les 5 premiers trucs d'information

    ImGui::Text("Operating System Used : %s", osName);
    ImGui::Text("Computer name : %s", hostname);
    ImGui::Text("User logged in : %s", user);
    ImGui::Text("CPU : %s", cpu.c_str());
    ImGui::Text("Number of working processes : %ld\n", tabProcess.size());
    ImGui::Separator();

    // Onglets
    if (ImGui::BeginTabBar("TabBar"))
    {
        if (ImGui::BeginTabItem("CPU"))
        {
            ImGui::Checkbox("Animate", &animate);
            ImGui::SliderInt("FPS", &system_FPSCounter, 1, 60);
            ImGui::SliderFloat("max scale", &system_graphScale, 0.001f, 100.f);
            char text[100];
            if (animate)
            {
                chrono::steady_clock::time_point CPU_end = chrono::steady_clock::now();
                chrono::duration<double> elapsed_seconds = chrono::duration_cast<chrono::duration<double>>(CPU_end - CPU_start);
                chrono::steady_clock::time_point display_end = chrono::steady_clock::now();
                chrono::duration<double> display_elapsed_seconds = chrono::duration_cast<chrono::duration<double>>(display_end - display_start);
                if (elapsed_seconds.count() >= 0.5)
                {
                    thread maj_CPUState(getCPUPercentage, std::ref(CPUPercent), system_FPSCounter);
                    maj_CPUState.detach();
                    CPU_start = CPU_end;
                }
                if (display_elapsed_seconds.count() >= 1.0 / system_FPSCounter)
                {
                    CPUState.push_back(CPUPercent);
                    display_start = display_end;
                }
                if (CPUState.size() >= 100)
                {
                    CPUState.erase(CPUState.begin());
                }
            }

            if (CPUPercent > 0.f)
                snprintf(text, sizeof(text), "Percentage : %.2f%%", CPUPercent);
            else
                snprintf(text, sizeof(text), "Calculation in progress...");

            ImGui::PlotLines("CPU", CPUState.data(), static_cast<int>(CPUState.size()), 0, text, 0.f, system_graphScale, ImVec2(0, 170));
            // free(hostname);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Fan"))
        {
            const char *fan_status = (char *)malloc(10);
            //getFanRPM(fanRPM);
            getFanInfos(fan_level);
            if (fanRPM > 0.f)
            {
                fan_status = "enabled";
            }
            else
            {
                fan_status = "disabled";
            }
            char text[100];
            if (animate)
            {
                chrono::steady_clock::time_point fan_end = chrono::steady_clock::now();
                chrono::duration<double> fan_elapsed_seconds = chrono::duration_cast<chrono::duration<double>>(fan_end - fan_start);
                if (fan_elapsed_seconds.count() >= 0.5)
                {
                    thread maj_fanRPM(getFanRPM, std::ref(fanRPM));
                    maj_fanRPM.detach();
                    fan_start = fan_end;
                }
                if (fan_elapsed_seconds.count() >= 1.0 / system_FPSCounter)
                {
                    fanState.push_back(fanRPM);
                    fan_start = fan_end;
                }
                if (fanState.size() >= 100)
                {
                    fanState.erase(fanState.begin());
                }
            }
            ImGui::Text("Fan status :");
            snprintf(text, sizeof(text), "\tSpeed : %.f", fanRPM);
            ImGui::Text("\tstatut : %s", fan_status);
            char status[100];
            snprintf(status, sizeof(status), "\tlevel : %s", fan_level.c_str());
            ImGui::Text(status);
            ImGui::Text(text);
            ImGui::Separator();
            ImGui::Checkbox("Animate", &animate);
            ImGui::SliderInt("FPS", &system_FPSCounter, 1, 60);
            ImGui::SliderFloat("scale max", &system_graphFanScale, 1000.f, 10000.f);
            ImGui::PlotLines("RPM", fanState.data(), static_cast<int>(fanState.size()), 0, text, 0.f, system_graphFanScale, ImVec2(0, 170));
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Thermal"))
        {
            ImGui::Checkbox("Animate", &animate);
            char text[100];
            if (animate)
            {
                chrono::steady_clock::time_point therm_end = chrono::steady_clock::now();
                chrono::duration<double> therm_elapsed_seconds = chrono::duration_cast<chrono::duration<double>>(therm_end - therm_start);
                if (therm_elapsed_seconds.count() >= 0.5)
                {
                    thread thermalState(getDeviceThermalState, std::ref(thermal_state));
                    thermalState.detach();
                    therm_start = therm_end;
                }
                if (therm_elapsed_seconds.count() >= 1.0 / system_FPSCounter)
                {
                    thermState.push_back(thermal_state);
                    therm_start = therm_end;
                }
                if (thermState.size() >= 100)
                {
                    thermState.erase(thermState.begin());
                }
            }
            snprintf(text, sizeof(text), "temp : %.f°C", thermal_state);
            ImGui::SliderInt("FPS", &system_FPSCounter, 1, 60);
            ImGui::SliderFloat("scale max", &system_graphScale, 0.f, 100.f);
            ImGui::PlotLines("THERMAL", thermState.data(), static_cast<int>(thermState.size()), 0, text, 0.f, system_graphScale, ImVec2(0, 220));
            ImGui::EndTabItem();
        }

        // proc/numéro du processus/status

        ImGui::EndTabBar();
    }

    ImGui::End();
}

// memoryProcessesWindow, display information for the memory and processes information
void memoryProcessesWindow(const char *id, ImVec2 size, ImVec2 position)
{
    ImGui::Begin(id);
    ImGui::SetWindowSize(id, size);
    ImGui::SetWindowPos(id, position);
    char text[100];
    thread mem(getMemState, std::ref(memState), ref(memTotal), ref(swapState), ref(swapTotal));
    mem.detach();

    snprintf(text, sizeof(text), "%.2f / %.2fGB", memState, memTotal / 1000000.f);
    ImGui::Text("Physic Memory (RAM) :");
    ImGui::ProgressBar((memState) / (memTotal / 1000000.f), ImVec2(550, 15), text);
    ImGui::Text("0 GB                                                                       %.2f GB", memTotal / 1000000.f);

    snprintf(text, sizeof(text), "%.2f / %.2fGB", swapState, swapTotal / 1000000.f);
    ImGui::Text("\nVirtual Memory (SWAP) :");
    ImGui::ProgressBar((swapState) / (swapTotal / 1000000.f), ImVec2(550, 15), text);
    ImGui::Text("0 GB                                                                       %.2f GB", swapTotal / 1000000.f);

    snprintf(text, sizeof(text), "%.0f / %.0fGB", totalSpace - freeSpace, totalSpace);
    ImGui::Text("\nDisk :");
    ImGui::ProgressBar((totalSpace - freeSpace) / totalSpace, ImVec2(550, 15), text);
    ImGui::Text("0 GB                                                                       %.0f GB", totalSpace);
    if (ImGui::BeginTabBar("ProcessesBar"))
    {
        if (ImGui::CollapsingHeader("Process Table"))
        {
            // char searchBuffer[256];
            ImGui::Text("filter the process by name:");
            string searchBuffer;
            ImGui::InputText("filter", &searchBuffer);

            ImGui::BeginTable("Process table", 5);
            ImGui::TableSetupColumn("PID");
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("State");
            ImGui::TableSetupColumn("CPU usage");
            ImGui::TableSetupColumn("Memory usage");
            ImGui::TableHeadersRow();

            for (auto proc : tabProcess)
            {
                if (searchBuffer.empty() || proc.name.find(searchBuffer) != string::npos)
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%d", proc.PID);

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", proc.name.c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%s", proc.state.c_str());

                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%.2f%%", proc.CPU);

                    ImGui::TableSetColumnIndex(4);
                    ImGui::Text("%.2f%%", (100 * proc.MEM) / memTotal);
                }
            }

            ImGui::EndTable();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

// network, display information network information
void networkWindow(const char *id, ImVec2 size, ImVec2 position)
{
    ImGui::Begin(id);
    ImGui::SetWindowSize(id, size);
    ImGui::SetWindowPos(id, position);

    auto now = std::chrono::system_clock::now();
    time_t time = std::chrono::system_clock::to_time_t(now);
    auto date = gmtime(&time);

    // Attention : lundi le programme risque de planter à cause du date->tm_wday-1
    ImGui::Text("%s %s %d %d:%d:%d %d", tabDay[date->tm_wday].data(), tabMonth[date->tm_mon].data(), date->tm_mday, date->tm_hour + 2, date->tm_min, date->tm_sec, 1900 + date->tm_year);
    ImGui::Text("");
    ImGui::Separator();

    // AF_INET = ipv4
    ip_addr = getIps();

    ImGui::Text("\nip4 network :");
    for (auto ip : ip_addr.ip4s)
    {
        ImGui::Text("\t%s : %s", ip.name, ip.addressBuffer);
    }
    ImGui::Text("");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Network table"))
    {
        if (ImGui::CollapsingHeader("RX"))
        {
            ImGui::BeginTable("RX table", 9);
            ImGui::TableSetupColumn("interface");
            ImGui::TableSetupColumn("bytes");
            ImGui::TableSetupColumn("packets");
            ImGui::TableSetupColumn("errs");
            ImGui::TableSetupColumn("drop");
            ImGui::TableSetupColumn("fifo");
            ImGui::TableSetupColumn("frame");
            ImGui::TableSetupColumn("compressed");
            ImGui::TableSetupColumn("multicast");
            ImGui::TableHeadersRow();

            for (auto net : ip_addr.ip4s)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", net.name);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%d", net.net_in.bytes);

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%d", net.net_in.packets);

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%d", net.net_in.errs);

                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%d", net.net_in.drop);

                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%d", net.net_in.fifo);

                ImGui::TableSetColumnIndex(6);
                ImGui::Text("%d", net.net_in.frame);

                ImGui::TableSetColumnIndex(7);
                ImGui::Text("%d", net.net_in.compressed);

                ImGui::TableSetColumnIndex(8);
                ImGui::Text("%d", net.net_in.multicast);
            }

            ImGui::EndTable();
        }
        if (ImGui::CollapsingHeader("TX"))
        {
            ImGui::BeginTable("TX table", 9);
            ImGui::TableSetupColumn("interface");
            ImGui::TableSetupColumn("bytes");
            ImGui::TableSetupColumn("packets");
            ImGui::TableSetupColumn("errs");
            ImGui::TableSetupColumn("drop");
            ImGui::TableSetupColumn("fifo");
            ImGui::TableSetupColumn("colls");
            ImGui::TableSetupColumn("carrier");
            ImGui::TableSetupColumn("compressed");
            ImGui::TableHeadersRow();

            for (auto net : ip_addr.ip4s)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s:", net.name);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%d", net.net_out.bytes);

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%d", net.net_out.packets);

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%d", net.net_out.errs);

                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%d", net.net_out.drop);

                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%d", net.net_out.fifo);

                ImGui::TableSetColumnIndex(6);
                ImGui::Text("%d", net.net_out.colls);

                ImGui::TableSetColumnIndex(7);
                ImGui::Text("%d", net.net_out.carrier);

                ImGui::TableSetColumnIndex(8);
                ImGui::Text("%d", net.net_out.compressed);
            }

            ImGui::EndTable();
        }
    }
    ImGui::Separator();

    if (ImGui::BeginTabBar("Network bar"))
    {
        if (ImGui::BeginTabItem("Receive(RX)"))
        {
            char text[100];
            for (auto net : ip_addr.ip4s)
            {
                ImGui::Text("%s:", net.name);
                float cast_value;
                string u;
                if (net.net_in.bytes >= 1000 / 8 && net.net_in.bytes <= 1000000 / 8)
                {
                    cast_value = static_cast<float>(net.net_in.bytes) / 1000 * 8;
                    u = "KB";
                }
                else if (net.net_in.bytes >= 1000000 / 8 && net.net_in.bytes <= 1000000000 / 8)
                {
                    cast_value = static_cast<float>(net.net_in.bytes) / 1000000 * 8;
                    u = "MB";
                }
                else if (net.net_in.bytes > 1000000000 / 8)
                {
                    cast_value = static_cast<float>(net.net_in.bytes) / 1000000000 * 8;
                    u = "GB";
                }
                else
                {
                    cast_value = net.net_in.bytes * 8;
                }
                snprintf(text, sizeof(text), "%.2f %s", cast_value, u.c_str());
                ImGui::ProgressBar(static_cast<float>(net.net_in.bytes) * 4.f / 1000000000.f, ImVec2(550, 15), text);
                ImGui::Text("0 GB                                                                       2 GB\n\n");
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Transmit(TX)"))
        {
            char text[100];
            for (auto net : ip_addr.ip4s)
            {
                ImGui::Text("%s:", net.name);
                float cast_value;
                string u = "KB";
                if (net.net_out.bytes >= 1000 / 8 && net.net_out.bytes <= 1000000 / 8)
                {
                    cast_value = static_cast<float>(net.net_out.bytes) / 1000 * 8;
                }
                else if (net.net_out.bytes >= 1000000 / 8 && net.net_out.bytes <= 1000000000 / 8)
                {
                    cast_value = static_cast<float>(net.net_out.bytes) / 1000000 * 8;
                    u = "MB";
                }
                else if (net.net_out.bytes > 1000000000 / 8)
                {
                    cast_value = static_cast<float>(net.net_out.bytes) / 1000000000 * 8;
                    u = "GB";
                }
                snprintf(text, sizeof(text), "%.2f %s", cast_value, u.c_str());
                ImGui::ProgressBar(static_cast<float>(net.net_out.bytes) * 4.f / 1000000000.f, ImVec2(550, 15), text);
                ImGui::Text("0 GB                                                                       2 GB\n\n");
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ip_addr.ip4s.clear();
    ImGui::End();
}

// Main code
int main(int, char **)
{
    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
    // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window *window = SDL_CreateWindow("Dear ImGui SDL2+OpenGL3 example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
    bool err = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress) == 0; // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
    bool err = false;
    glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
    bool err = false;
    glbinding::initialize([](const char *name)
                          { return (glbinding::ProcAddress)SDL_GL_GetProcAddress(name); });
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // render bindings
    ImGuiIO &io = ImGui::GetIO();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // background color
    // note : you are free to change the style of the application
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    hostname = (char *)malloc(10);
    gethostname(hostname, 10);
    osName = getOsName();
    user = getenv("USER");
    cpu = CPUinfo();
    getHardDiskSpace(totalSpace, freeSpace);
    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        getProcesses(tabProcess);
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        {
            ImVec2 mainDisplay = io.DisplaySize;
            memoryProcessesWindow("== Memory and Processes ==",
                                  ImVec2((mainDisplay.x / 2) - 20, (mainDisplay.y / 2) + 30),
                                  ImVec2((mainDisplay.x / 2) + 10, 10));
            // --------------------------------------
            systemWindow("== System ==",
                         ImVec2((mainDisplay.x / 2) - 10, (mainDisplay.y / 2) + 30),
                         ImVec2(10, 10));
            // --------------------------------------
            networkWindow("== Network ==",
                          ImVec2(mainDisplay.x - 20, (mainDisplay.y / 2) - 60),
                          ImVec2(10, (mainDisplay.y / 2) + 50));
        }

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
