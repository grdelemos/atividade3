# Atividade3
Computação Gráfica 


Gustavo Gualberto Rocha de Lemos 


 **RA**: 21008313

Explore uma galáxia com o Galaxy Explorer! O aplicativo permite explorar uma galáxia, controlando livremente a câmera e se movimentando pelo espaço 3D usando o mouse e o teclado. Existem naves viajando, satélites a deriva, uma estação espacial, um planeta e estrelas distantes, e cada elemento se move de forma particular no ambiente. Também pode ser aplicado zoom positivo e negativo na visualização, o que pode causar um efeito Doppler nas cores dos elementos visíveis. 

A câmera é criada usando a função glm::LookAT:

    m_viewMatrix = glm::lookAt(eye, eye + at, up);

Onde 'eye' é a posição da câmera, 'eye+at' é o vetor que indica a direção para onde a câmera está olhando, e up é o vetor de orientação "para cima".
Os valores iniciais desses parâmetros são:

    glm::vec3 eye{0.0f, 0.0f, 0.0f};
    glm::vec3 at{0.0f, 0.0f, -1.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};

Então inicialmente a câmera estará na posição 0,0,0, olhando para 0,0,-1, tendo como orientação "para cima" o vetor 0,1,0.

O módulo Window contém o funcionamento principal da aplicação:
A função onEvent lida com as entradas do usuário. Ela contém uma constante chamada "speed", que é o valor 5 multiplicado por deltatime, que é o tempo entre um quadro e outro. Esse multiplicador é aplicado a todas as movimentações da câmera para uma movimentação numa velocidade num ritmo adequado.
As teclas a, s, d, w, r, f são usadas pelo programa da seguinte maneira:

    if (event.key.keysym.sym == SDLK_a)
    eye -= glm::normalize(glm::cross(at, up)) * speed;
    if (event.key.keysym.sym == SDLK_d)
    eye += glm::normalize(glm::cross(at, up)) * speed;

As teclas 'a' e 'd' fazem a posição da câmera mudar lateralmente. Isso acontece ao incrementar ou decrementar o valor de 'eye', que é a posição da câmera, com um vetor normal à multiplicação vetorial entre at e up. Isso funciona porque a multiplicação vetorial entre dois vetores resulta em um vetor perpendicular aos dois, que nesse caso entre at e up, será um vetor lateral à posição da câmera.

As teclas 'w' e 's' avançam ou recuam a câmera na direção em que está olhando:

    if (event.key.keysym.sym == SDLK_w)
        eye += speed * at;
    if (event.key.keysym.sym == SDLK_s)
        eye -= speed * at;

Isso é feito incrementando ou decrementando o valor 'eye' com o vetor 'at', sempre multiplicado por 'speed'.

Por fim, as teclas 'r' e 'f' mudam a posição verticalmente:

    if (event.key.keysym.sym == SDLK_r)
        eye += speed * up;
    if (event.key.keysym.sym == SDLK_f)
        eye -= speed * up;

Com elas a posição é somada ou subtraída com o vetor up.

O mouse wheel controla o valor do ângulo de campo de visão, field of view:

    if (event.type == SDL_MOUSEWHEEL) {
        if (event.wheel.y > 0 && m_FOV > 15.0f)
        m_FOV -= 1.0f;
        if (event.wheel.y < 0 && m_FOV < 70.0f)
        m_FOV += 1.0f;
    }

O ângulo é usado para dar um efeito de zoom, e é limitado para ficar entre os valores de 15 até 70. Esse valor também é usado pelo shader para saber quando aplicar o efeito Doppler.

A posição do mouse é usada para calcular a direção para onde olhar:

    if (event.type == SDL_MOUSEMOTION) {
        SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
    }
Essa função guarda as coordenadas do mouse na variável mousePosition.

A função onCreate define os parâmetros iniciais da aplicação, como a localização dos shaders e modelos na pasta Assets, a câmera e as posições e rotações dos objetos em cena.

    m_program =
    abcg::createOpenGLProgram({{.source = assetsPath + "depth.vert",
                                .stage = abcg::ShaderStage::Vertex},
                                {.source = assetsPath + "depth.frag",
                                .stage = abcg::ShaderStage::Fragment}});

m_program será usado para renderizar os objetos, e os seus vertex e fragment shader estão na pasta assets.

Para a criação dos modelos é usado o módulo Model, com cada modelo sendo um objeto dele. 
Por exemplo, para o planeta:

    planet.loadObj(assetsPath + "planet.obj");
    planet.setupVAO(m_program);

O módulo Model usa a função loadObj para carregar o arquivo "planet.obj", e também cria o vertex array object para ele dentro de m_program.

m_viewMatrix define a posição inicial da câmera, por onde o usuário verá toda a cena:

    m_viewMatrix = glm::lookAt(eye, eye + at, up);

O seguinte loop define a posição e rotação de cada estrela:

    for (auto &star : m_stars) {
        randomizeStar(star);
    }

m_stars é um array com 1000 structs de nome Star, que têm uma posição e um eixo de rotação cada uma. A função randomizeStar define uma posição e um eixo de rotação para cada uma:

    std::uniform_real_distribution<float> distPosXY(-50.0f, 50.0f);
    std::uniform_real_distribution<float> distPosZ(-150.0f, -50.0f);
    star.m_position =
        glm::vec3(distPosXY(m_randomEngine), distPosXY(m_randomEngine),
                    distPosZ(m_randomEngine));

    // Random rotation axis
    star.m_rotationAxis = glm::sphericalRand(1.0f);

a posição delas é definida aleatoriamente, mas com os eixos x e y variando entre -50 e 50, e o eixo z variando entre -150 e -50. Esses valores no eixo z garantem que as estrelas estarão distantes da câmera no início da aplicação. O eixo de rotação das estrelas é definido aleatoriamente.

Por fim, a posição e eixo dos demais objetos é definida, cada um deles também sendo uma Struct com um valor de posição e outro de eixo de rotação:

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

A posição inicial desses objetos é fixa, e foi pensada para criar uma boa primeira visão para o usuário. O planeta, o satelite e a sonda têm eixos aleatórios, e os demais têm eixos fixos, definidos para melhor visualização.

A função onUpdate lida com as mudanças a cada quadro:

    m_angle = glm::wrapAngle(m_angle + glm::radians(90.0f) * deltaTime);

m_angle é a variável que vai mudar a cada segundo, fazendo com que os modelos que têm sua rotação ligada a ela girem 90 graus por segundo.

Para fazer a movimentação do mouse mudar a direção para onde a câmera olha é necessário que se façam as seguintes operações trigonométricas:

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


xoffset e yoffset guardam a diferença da posição do mouse entre um quadro e outro, e são multiplicados pelo valor de sensitivity, que define a velocidade que a câmera vai se movimentar. O valor pitch é o eixo de rotação vertical, e yaw é o eixo horizontal. A direção da câmera é definida pelas operações trigonométricas entre elas: O valor em x é o cosseno do eixo horizontal multiplicado pelo cosseno do eixo vertical, o valor em y é o seno do eixo vertical, e o valor z é o seno do eixo horizontal multiplicado pelo cosseno do eixo vertical.

Os objetos se movem pela cena em direções e velocidades variadas:

    m_satelite1.m_position += deltaTime * glm::vec3{0.01f, 0.01f, -0.01f};
    m_probe.m_position += deltaTime * glm::vec3{-0.2f, 0.3f, -0.02f};
    m_ship.m_position += deltaTime * glm::vec3{-0.3f, 0.2f, -0.2f};
    m_ufo.m_position += deltaTime * glm::vec3{0.3f, 0.02f, -0.2f};
    m_station.m_position += deltaTime * glm::vec3{0.02f, 0.02f, -0.02f};

Essas movimentações foram escolhidas para criar uma cena mais dinâmica, com objetos se movimentando em direções diferentes, objetos à deriva e objetos se movendo lentamente.

Também é chamada a função CheckCollision, que evita que a câmera encoste nos objetos. Para isso ela calcula a distância entre a câmera e o objeto, e caso a distância seja menor que 1 + a escala do objeto, ela "empurra" a câmera para trás. Por exemplo, o satélite:

    auto const distanceSatelite{glm::distance(eye, m_satelite1.m_position)};
    if (distanceSatelite < 1.8f) {

        eye -= 5.0f * deltaTime * at;
    }

Se a distância entre o satelite e a câmera for menor que 1.8, a câmera se move para trás, na mesma direção do vetor at.

A função onPaint faz a renderização dos objetos, usando matrizes e os shaders. Cada modelo tem as suas particularidades, por exemplo o UFO:

    glm::mat4 modelMatrixUfo{1.0f};
    modelMatrixUfo = glm::translate(modelMatrixUfo, m_ufo.m_position);
    modelMatrixUfo = glm::scale(modelMatrixUfo, glm::vec3(0.8f));
    modelMatrixUfo =
        glm::rotate(modelMatrixUfo, 80.0f, glm::vec3{1.0f, 0.0f, 0.0f});
    modelMatrixUfo = glm::rotate(modelMatrixUfo, m_angle, m_ufo.m_rotationAxis);

a matriz do modelo passa pela função glm::translate, que indica a posição de acordo com o valor em m_position, a função glm::scale indica o tamanho do modelo e por fim o modelo é girado de acordo com os valores da variável m_angle.

O fragment shader depth.frag faz o efeito Doppler:

    float x, y, z, w;
        x = fragColor.x;
        y = fragColor.y;
        z = fragColor.z;
        w = 1.0;

        if (fieldDoppler > 55.0){
            
            y -= 0.2;
            z -= 0.2;
        }

        if (fieldDoppler < 17.0){
            
            x -= 0.2;
            y -= 0.2;
        }
    outColor = vec4(x, y, z, w); 

O shader recebe os valores da cor do modelo no formato RGBA, e o ângulo do campo de visão. Se o campo de visão for maior que 55, os valores de verde (G) e azul (B) da cor de todos os modelos são reduzidos em 0.2, o que faz com que eles tenham a cor mais avermelhada. Se o campo de visão for menor que 17, os valores de vermelho (R) e verde (G) são reduzidos, para fazer os modelos ficarem azuis.

A UI tem um quadro no canto da tela, com um botão para voltar a câmera a sua posição inicial, e um botão para mostrar os controles:

    if (ImGui::Button("Voltar ao Início", ImVec2(0, 0))) {
            eye = glm::vec3{0.0f, 0.0f, 0.0f};
        }
        if (ImGui::Button("Mostrar Controles", ImVec2(0, 0))) {
            control = !control;
        }

Se o botão "Voltar ao início" for apertado, a câmera volta à posição 0,0,0, e se o botão "Mostrar Controles" for pressionado, a variável booleana control, que é iniciada com valor false, tem seu valor invertido, ficando true. Isso faz a janela que mostra os controles aparecer:

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

Ela mostra a janela com todos os comandos, e um botão "Ocultar Controles", que inverte o valor de control novamente, fazendo a janela original voltar a aparecer.
    