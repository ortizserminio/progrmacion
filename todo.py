import json
import os

ARCHIVO = "tareas.json"


def cargar_tareas():
    if os.path.exists(ARCHIVO):
        with open(ARCHIVO, "r") as f:
            return json.load(f)
    return []


def guardar_tareas(tareas):
    with open(ARCHIVO, "w") as f:
        json.dump(tareas, f, indent=2, ensure_ascii=False)


def mostrar_tareas(tareas):
    if not tareas:
        print("No hay tareas.")
        return
    print("\n--- Mis Tareas ---")
    for i, t in enumerate(tareas, 1):
        estado = "✓" if t["completada"] else "○"
        print(f"  {i}. [{estado}] {t['nombre']}")
    print()


def agregar_tarea(tareas, nombre):
    tareas.append({"nombre": nombre, "completada": False})
    guardar_tareas(tareas)
    print(f"Tarea '{nombre}' añadida.")


def completar_tarea(tareas, numero):
    if 1 <= numero <= len(tareas):
        tareas[numero - 1]["completada"] = True
        guardar_tareas(tareas)
        print(f"Tarea {numero} marcada como completada.")
    else:
        print("Número de tarea inválido.")


def borrar_tarea(tareas, numero):
    if 1 <= numero <= len(tareas):
        eliminada = tareas.pop(numero - 1)
        guardar_tareas(tareas)
        print(f"Tarea '{eliminada['nombre']}' borrada.")
    else:
        print("Número de tarea inválido.")


def menu():
    print("1. Ver tareas")
    print("2. Agregar tarea")
    print("3. Completar tarea")
    print("4. Borrar tarea")
    print("5. Salir")


def main():
    tareas = cargar_tareas()
    print("=== To-Do List ===")
    while True:
        menu()
        opcion = input("Elige una opción: ").strip()
        if opcion == "1":
            mostrar_tareas(tareas)
        elif opcion == "2":
            nombre = input("Nombre de la tarea: ").strip()
            if nombre:
                agregar_tarea(tareas, nombre)
        elif opcion == "3":
            mostrar_tareas(tareas)
            try:
                num = int(input("Número a completar: "))
                completar_tarea(tareas, num)
            except ValueError:
                print("Ingresa un número válido.")
        elif opcion == "4":
            mostrar_tareas(tareas)
            try:
                num = int(input("Número a borrar: "))
                borrar_tarea(tareas, num)
            except ValueError:
                print("Ingresa un número válido.")
        elif opcion == "5":
            print("¡Hasta luego!")
            break
        else:
            print("Opción no válida.")


if __name__ == "__main__":
    main()
