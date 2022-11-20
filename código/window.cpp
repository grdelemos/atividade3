#include "window.hpp"

#include <glm/gtc/random.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Window::onEvent(SDL_Event const &event) {

  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};
  float speed = 5.0f * deltaTime;
  // Keyboard events
  if (event.type == SDL_KEYDOWN) {

    if (event.key.keysym.sym == SDLK_a)
      eye -= glm::normalize(glm::cross(at, up)) * speed;
    if (event.key.keysym.sym == SDLK_d)
      eye += glm::normalize(glm::cross(at, up)) * speed;
    if (event.key.keysym.sym == SDLK_w)
      eye += speed * at;
    if (event.key.keysym.sym == SDLK_s)
      eye -= speed * at;
    if (event.key.keysym.sym == SDLK_r)
      eye += speed * up;
    if (event.key.keysym.sym == SDLK_f)
      eye -= speed * up;
  }

  if (event.type == SDL_MOUSEWHEEL) {
    if (event.wheel.y > 0 && m_FOV > 15.0f)
      m_FOV -= 1.0f;
    if (event.wheel.y < 0 && m_FOV < 70.0f)
      m_FOV += 1.0f;
  }

  if (event.type == SDL_MOUSEMOTION) {
    SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
  }
}

void Window::onCreate() {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0, 0, 0, 1);
  abcg::glEnable(GL_DEPTH_TEST);

  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "depth.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "depth.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  m_model.loadObj(assetsPath + "star.obj");
  m_model.setupVAO(m_program);

  satelite1.loadObj(assetsPath + "satelite1.obj");
  satelite1.setupVAO(m_program);

  probe.loadObj(assetsPath + "probe.obj");
  probe.setupVAO(m_program);

  planet.loadObj(assetsPath + "planet.obj");
  planet.setupVAO(m_program);

  ship.loadObj(assetsPath + "ship.obj");
  ship.setupVAO(m_program);

  station.loadObj(assetsPath + "station.obj");
  station.setupVAO(m_program);

  ufo.loadObj(assetsPath + "ufo.obj");
  ufo.setupVAO(m_program);

  // Camera at (0,0,0) and looking towards the negative z

  m_viewMatrix = glm::lookAt(eye, eye + at, up);

  // Setup stars
  for (auto &star : m_stars) {
    randomizeStar(star);
  }

  m_satelite1.m_position = glm::vec3(-10.0f, 15.0f, -40.0f);
  m_satelite1.m_rotationAxis = glm::sphericalRand(1.0f);

  m_probe.m_position = glm::vec3(10.0f, -5.0f, -30.0f);
  m_probe.m_rotationAxis = glm::sphericalRand(1.0f);

  m_planet.m_position = glm::vec3(20.0f, 15.0f, -90.0f);
  m_planet.m_rotationAxis = glm::sphericalRand(1.0f);

  m_ship.m_position = glm::vec3(10.0f, 0.0f, -20.0f);
  m_ship.m_rotationAxis = glm::vec3(1.0f, 1.0f, 0.0f);

  m_station.m_position = glm::vec3(-2.0f, 10.0f, -40.0f);
  m_station.m_rotationAxis = glm::vec3(-1.0f, -1.0f, 0.0f);

  m_ufo.m_position = glm::vec3(-10.0f, 1.0f, -20.0f);
  m_ufo.m_rotationAxis = glm::vec3(0.0f, 0.0f, 1.0f);
}

void Window::randomizeStar(Star &star) {

  std::uniform_real_distribution<float> distPosXY(-50.0f, 50.0f);
  std::uniform_real_distribution<float> distPosZ(-150.0f, -50.0f);
  star.m_position =
      glm::vec3(distPosXY(m_randomEngine), distPosXY(m_randomEngine),
                distPosZ(m_randomEngine));

  // Random rotation axis
  star.m_rotationAxis = glm::sphericalRand(1.0f);
}

void Window::onUpdate() {
  // Increase angle by 90 degrees per second
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};
  m_angle = glm::wrapAngle(m_angle + glm::radians(90.0f) * deltaTime);

  float xoffset = mousePosition.x - lastX;
  float yoffset = lastY - mousePosition.y;
  lastX = mousePosition.x;
  lastY = mousePosition.y;

  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  glm::vec3 direction;
  direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.y = sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  at = glm::normalize(direction);

  m_viewMatrix = glm::lookAt(eye, eye + at, up);

  m_satelite1.m_position += deltaTime * glm::vec3{0.01f, 0.01f, -0.01f};
  m_probe.m_position += deltaTime * glm::vec3{-0.2f, 0.3f, -0.02f};
  m_ship.m_position += deltaTime * glm::vec3{-0.3f, 0.2f, -0.2f};
  m_ufo.m_position += deltaTime * glm::vec3{0.3f, 0.02f, -0.2f};
  m_station.m_position += deltaTime * glm::vec3{0.02f, 0.02f, -0.02f};

  checkCollision();
}

void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  abcg::glUseProgram(m_program);

  auto const aspect{gsl::narrow<float>(m_viewportSize.x) /
                    gsl::narrow<float>(m_viewportSize.y)};
  m_projMatrix = glm::perspective(glm::radians(m_FOV), aspect, 0.01f, 100.0f);

  // Get location of uniform variables
  auto const viewMatrixLoc{abcg::glGetUniformLocation(m_program, "viewMatrix")};
  auto const projMatrixLoc{abcg::glGetUniformLocation(m_program, "projMatrix")};
  auto const modelMatrixLoc{
      abcg::glGetUniformLocation(m_program, "modelMatrix")};
  auto const colorLoc{abcg::glGetUniformLocation(m_program, "color")};
  auto field{abcg::glGetAttribLocation(m_program, "field")};

  // Set uniform variables that have the same value for every model
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  abcg::glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f); // White
  abcg::glVertexAttrib1f(field, m_FOV);

  // Render each star
  for (auto &star : m_stars) {
    // Compute model matrix of the current star
    glm::mat4 modelMatrix{1.0f};
    modelMatrix = glm::translate(modelMatrix, star.m_position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
    modelMatrix = glm::rotate(modelMatrix, m_angle, star.m_rotationAxis);

    // Set uniform variable
    abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

    m_model.render();
  }

  // satelite
  glm::mat4 modelMatrix{1.0f};
  modelMatrix = glm::translate(modelMatrix, m_satelite1.m_position);
  modelMatrix = glm::scale(modelMatrix, glm::vec3(0.8f));
  modelMatrix = glm::rotate(modelMatrix, m_angle, m_satelite1.m_rotationAxis);

  abcg::glUniform4f(colorLoc, 0.5f, 0.5f, 0.5f, 1.0f);
  // Set uniform variable
  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

  satelite1.render();

  // planet
  glm::mat4 modelMatrixPlanet{1.0f};
  modelMatrixPlanet = glm::translate(modelMatrixPlanet, m_planet.m_position);
  modelMatrixPlanet = glm::scale(modelMatrixPlanet, glm::vec3(13.0f));
  modelMatrixPlanet =
      glm::rotate(modelMatrixPlanet, m_angle, m_planet.m_rotationAxis);

  abcg::glUniform4f(colorLoc, 1.0f, 0.0f, 0.3f, 1.0f);
  // Set uniform variable
  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE,
                           &modelMatrixPlanet[0][0]);

  planet.render();

  // probe
  glm::mat4 modelMatrixProbe{1.0f};
  modelMatrixProbe = glm::translate(modelMatrixProbe, m_probe.m_position);
  modelMatrixProbe = glm::scale(modelMatrixProbe, glm::vec3(0.8f));
  modelMatrixProbe =
      glm::rotate(modelMatrixProbe, m_angle, m_probe.m_rotationAxis);

  abcg::glUniform4f(colorLoc, 0.5f, 0.5f, 0.5f, 1.0f);
  // Set uniform variable
  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE,
                           &modelMatrixProbe[0][0]);

  probe.render();

  // ship
  glm::mat4 modelMatrixShip{1.0f};
  modelMatrixShip = glm::translate(modelMatrixShip, m_ship.m_position);
  modelMatrixShip = glm::scale(modelMatrixShip, glm::vec3(0.8f));
  modelMatrixShip = glm::rotate(modelMatrixShip, 45.0f, m_ship.m_rotationAxis);

  abcg::glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
  // Set uniform variable
  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrixShip[0][0]);

  ship.render();

  // station
  glm::mat4 modelMatrixStation{1.0f};
  modelMatrixStation = glm::translate(modelMatrixStation, m_station.m_position);
  modelMatrixStation = glm::scale(modelMatrixStation, glm::vec3(6.0f));
  modelMatrixStation =
      glm::rotate(modelMatrixStation, 90.0f, m_station.m_rotationAxis);

  // Set uniform variable
  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE,
                           &modelMatrixStation[0][0]);

  station.render();

  // ufo
  glm::mat4 modelMatrixUfo{1.0f};
  modelMatrixUfo = glm::translate(modelMatrixUfo, m_ufo.m_position);
  modelMatrixUfo = glm::scale(modelMatrixUfo, glm::vec3(0.8f));
  modelMatrixUfo =
      glm::rotate(modelMatrixUfo, 80.0f, glm::vec3{1.0f, 0.0f, 0.0f});
  modelMatrixUfo = glm::rotate(modelMatrixUfo, m_angle, m_ufo.m_rotationAxis);

  // Set uniform variable
  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrixUfo[0][0]);

  ufo.render();

  abcg::glUseProgram(0);
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  if (control) {
    ImGuiWindowFlags const flags{ImGuiWindowFlags_NoNavInputs |
                                 ImGuiWindowFlags_NoDecoration};

    ImGui::SetNextWindowPos(
        ImVec2(m_viewportSize.x / 5.0f, (m_viewportSize.y) / 80.0f));
    ImGui::SetNextWindowSize(ImVec2(250, 170));
    ImGui::Begin("Widget window", nullptr, flags);
    ImGui::Text("tecla a = Mover para a esquerda");
    ImGui::Text("tecla d = Mover para a direita");
    ImGui::Text("tecla w = Mover para frente");
    ImGui::Text("tecla s = Mover para trás");
    ImGui::Text("tecla r = Mover para cima");
    ImGui::Text("tecla f = Mover para baixo");
    ImGui::Text("scroll wheel = Zoom");

    if (ImGui::Button("Ocultar Controles", ImVec2(0, 0))) {
      control = !control;
    }
    ImGui::End();
  }

  {
    auto widgetSize{ImVec2(150, 60)};
    ImGui::SetNextWindowPos(
        ImVec2(m_viewportSize.x / 4.0f, (m_viewportSize.y) / 80.0f));
    ImGui::SetNextWindowSize(widgetSize);

    ImGuiWindowFlags const flags{ImGuiWindowFlags_NoNavInputs |
                                 ImGuiWindowFlags_NoDecoration};

    ImGui::Begin("Widget window", nullptr, flags);

    {

      if (ImGui::Button("Voltar ao Início", ImVec2(0, 0))) {
        eye = glm::vec3{0.0f, 0.0f, 0.0f};
      }
      if (ImGui::Button("Mostrar Controles", ImVec2(0, 0))) {
        control = !control;
      }
    }

    ImGui::End();
  }
}

void Window::onResize(glm::ivec2 const &size) { m_viewportSize = size; }

void Window::checkCollision() {

  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  for (auto &star : m_stars) {
    auto const distance{glm::distance(eye, star.m_position)};

    if (distance < 1.0f + 0.2f * 0.85f) {

      eye -= 5.0f * deltaTime * at;
    }
  }

  auto const distanceSatelite{glm::distance(eye, m_satelite1.m_position)};
  if (distanceSatelite < 1.8f) {

    eye -= 5.0f * deltaTime * at;
  }

  auto const distanceProbe{glm::distance(eye, m_probe.m_position)};
  if (distanceProbe < 1.8f) {

    eye -= 5.0f * deltaTime * at;
  }

  auto const distanceShip{glm::distance(eye, m_ship.m_position)};
  if (distanceShip < 1.8f) {

    eye -= 5.0f * deltaTime * at;
  }

  auto const distanceStation{glm::distance(eye, m_station.m_position)};
  if (distanceStation < 7.0f) {

    eye -= 5.0f * deltaTime * at;
  }

  auto const distanceUfo{glm::distance(eye, m_ufo.m_position)};
  if (distanceUfo < 1.8f) {

    eye -= 5.0f * deltaTime * at;
  }

  auto const distancePlanet{glm::distance(eye, m_planet.m_position)};
  if (distancePlanet < 14.0f) {

    eye -= 5.0f * deltaTime * at;
  }
}

void Window::onDestroy() {
  m_model.destroy();
  abcg::glDeleteProgram(m_program);
}