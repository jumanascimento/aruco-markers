import cv2
import numpy as np

# Variáveis globais
drawing = False  # Indica se o mouse está sendo pressionado para desenhar a reta
ix, iy = -1, -1  # Coordenadas iniciais da reta

# Variáveis de distância e altura do objeto
focal_length = 0.0
object_height = 0.0
sensor_height = 0.0

# Função de callback do mouse para desenhar a reta
def draw_line(event, x, y, flags, param):
    global ix, iy, drawing

    if event == cv2.EVENT_LBUTTONDOWN:
        drawing = True
        ix, iy = x, y

    elif event == cv2.EVENT_LBUTTONUP:
        drawing = False
        cv2.line(image, (ix, iy), (x, y), (0, 255, 0), 2)
        cv2.imshow('Image', image)

        # Calcular a altura do objeto em pixels
        object_height_px = abs(y - iy)
        calculate_distance(object_height_px)


# Função para calcular a distância
def calculate_distance(object_height_px):
    distance_m = (focal_length * object_height * image_height) / (object_height_px * sensor_height)
    print("Distância:", distance_m, "metros")


# Definir a altura do objeto fornecida pelo usuário
object_height = float(input("Digite a altura do objeto em metros: "))

# Definir os parâmetros da câmera
focal_length = float(input("Digite o comprimento focal em pixels: "))
sensor_height = float(input("Digite a altura do sensor em milímetros: "))

# Carregar a imagem
image = cv2.imread('/home/julia/Documents/click_test/image/teste.jpg')
image_height = image.shape[0]  # Altura da imagem em pixels

# Criar uma janela e configurar o callback do mouse para desenhar a reta
cv2.namedWindow('Image')
cv2.setMouseCallback('Image', draw_line)

while True:
    cv2.imshow('Image', image)
    key = cv2.waitKey(1)
    if key == ord('q') or key == 27:  # Pressione 'q' ou ESC para sair
        break

cv2.destroyAllWindows()

