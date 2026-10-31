#pragma once

#include <QtWidgets/QMainWindow>
#include "ArchivosContactos.h"

// Declaración adelantada del namespace Ui para que C++ lo reconozca
QT_BEGIN_NAMESPACE
namespace Ui { class Labo7; }
QT_END_NAMESPACE

class Labo7 : public QMainWindow
{
    Q_OBJECT

public:
    Labo7(QWidget* parent = nullptr);
    ~Labo7();

private slots:
    // Slots para los 5 botones de navegación superiores
    void irANavegacion();
    void irARegistro();
    void irAlHistorial();
    void irAEditar();
    void irAEliminar();

    // Slots para la gestión de contactos y tabla
    void guardarContacto();
    void cargarTablaContactos();
    void eliminarContacto();
    void eliminarContactoSeleccionado();
    void editarContactoSeleccionado();

private:
    Ui::Labo7* ui;
    ArchivoContactos archivoContactos;
    int filaEditando = -1; // -1 = Nuevo contacto, >= 0 = Editando esa fila
};