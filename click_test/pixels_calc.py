import cv2

# Variáveis globais
drawing = False  # Indica se o mouse está sendo pressionado para desenhar o retângulo
ix, iy = -1, -1  # Coordenadas iniciais do retângulo

# Função de callback do mouse
def draw_rectangle(event, x, y, flags, param):
    global ix, iy, drawing

    if event == cv2.EVENT_LBUTTONDOWN:
        drawing = True
        ix, iy = x, y

    elif event == cv2.EVENT_LBUTTONUP:
        drawing = False
        cv2.rectangle(image, (ix, iy), (x, y), (0, 255, 0), 2)
        cv2.imshow('image', image)

        # Calcular quantidade de pixels no retângulo
        pixels = (x - ix) * (y - iy)
        print("Quantidade de pixels:", pixels)


# Carregar a imagem
image = cv2.imread('/home/julia/Documents/click_test/image/teste.jpg')

# Criar uma janela e configurar o callback do mouse
cv2.namedWindow('image')
cv2.setMouseCallback('image', draw_rectangle)

while True:
    cv2.imshow('image', image)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cv2.destroyAllWindows()

