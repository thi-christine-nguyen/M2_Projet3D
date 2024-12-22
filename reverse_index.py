def reverse_face_indices(input_file, output_file):
    with open(input_file, 'r') as file:
        lines = file.readlines()

    # Vérifie que le fichier est bien au format OFF
    if lines[0].strip() != "OFF":
        print("Le fichier n'est pas au format OFF.")
        return

    # Récupère le nombre de sommets, de faces et d'arêtes
    vertex_count, face_count, edge_count = map(int, lines[1].strip().split())

    vertices = []
    faces = []

    # Lire les sommets
    for i in range(2, 2 + vertex_count):
        vertex_data = lines[i].strip()
        vertices.append(vertex_data)

    # Lire les faces et inverser l'ordre des indices des sommets
    for i in range(2 + vertex_count, 2 + vertex_count + face_count):
        face_data = list(map(int, lines[i].strip().split()))
        num_vertices = face_data[0]

        # Vérifie que la face contient bien des indices (3 dans ce cas)
        if num_vertices != 3:
            print(f"Attention, la face à la ligne {i+1} ne contient pas 3 indices.")
            return

        # Inverser l'ordre des indices des sommets
        face_data[1], face_data[2], face_data[3] = face_data[3], face_data[2], face_data[1]

        faces.append(face_data)

    # Écrire le nouveau fichier .off avec les indices réorientés
    with open(output_file, 'w') as file:
        file.write("OFF\n")
        file.write(f"{vertex_count} {face_count} {edge_count}\n")

        # Écrire les sommets inchangés
        for vertex in vertices:
            file.write(f"{vertex}\n")

        # Écrire les faces avec l'ordre des indices inversé
        for face in faces:
            file.write(f"{face[0]} {face[1]} {face[2]} {face[3]}\n")

    print(f"Le fichier {output_file} a été créé avec les indices des faces inversés.")

# Exemple d'utilisation
input_file = "bear.off"
output_file = "output_reversed.off"
reverse_face_indices(input_file, output_file)
