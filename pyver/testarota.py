with open("teste.txt", "r") as f:
    demandas = [int(x) for x in f.readline().split()]
    rota = [int(x) for x in f.readline().split()]

    print(demandas)
    print(rota)

    capacidade = 20
    carga = 0
    min = demandas[0]
    max = min

    for i in range(1, len(rota)-1):
        carga += demandas[rota[i]-1]
        if carga > capacidade:
            print("Invalida")
            exit(0)
        
        if carga > max:
            max = carga
        
        if carga < min:
            min = carga

    min = -min
    max = capacidade - max

    if min > max or max < 0 or min > capacidade:
        print("Invalida")

    i = max
    while(i >= min):
        if i <= capacidade:
            print("Valida")
            print(i)
            exit(0)
        i -= 1

    print("Invalida")