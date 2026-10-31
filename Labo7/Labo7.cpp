#include "Labo7.h"
#include "ui_Labo7.h"
#include <QPushButton>
#include <QLineEdit>
#include <QMessageBox>
#include <QStringList>
#include <QAbstractItemView>
#include <QFile>
#include <QTextStream>
#include <QTableWidgetItem>
#include <QAction>

// Constructor
Labo7::Labo7(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::Labo7)
{
    ui->setupUi(this);

    // --- 1. CONEXIONES DE LOS 5 BOTONES SUPERIORES (Navegación entre páginas) ---
    connect(ui->btnNavegacion, &QPushButton::clicked, this, &Labo7::irANavegacion);
    connect(ui->btnRegistrar, &QPushButton::clicked, this, &Labo7::irARegistro);
    connect(ui->btnHistorial, &QPushButton::clicked, this, &Labo7::irAlHistorial);
    connect(ui->btnEditar, &QPushButton::clicked, this, &Labo7::irAEditar);
    connect(ui->btnEliminar, &QPushButton::clicked, this, &Labo7::irAEliminar);

    // --- 2. CONEXIONES DE LOS FORMULARIOS Y ACCIONES ---
    connect(ui->btnGuardar, &QPushButton::clicked, this, &Labo7::guardarContacto);

    // --- 3. CONEXIONES DE LOS 2 BOTONES INFERIORES (En la vista de tabla/historial) ---
    connect(ui->btnEliminarSeleccionado, &QPushButton::clicked, this, &Labo7::eliminarContactoSeleccionado);
    connect(ui->btnEditarSeleccionado, &QPushButton::clicked, this, &Labo7::editarContactoSeleccionado);
}

// Destructor
Labo7::~Labo7()
{
    delete ui;
}

// --- MÉTODOS DE NAVEGACIÓN ---
void Labo7::irANavegacion() {
    ui->stackedWidget->setCurrentIndex(0); // Ajusta el índice según tu orden de páginas
}

void Labo7::irARegistro() {
    filaEditando = -1; // Indicamos que es un registro nuevo, no una edición
    ui->txtNombre->clear();
    ui->txtTelefono->clear();
    ui->txtCorreo->clear();
    ui->stackedWidget->setCurrentIndex(1); // Índice de tu página de registro
}

void Labo7::irAlHistorial() {
    ui->stackedWidget->setCurrentIndex(2); // Asegúrate de que coincida con el índice de tu tabla
    cargarTablaContactos(); // Carga los datos al entrar al historial
}

void Labo7::irAEditar() {
    ui->stackedWidget->setCurrentIndex(3);
}

void Labo7::irAEliminar() {
    ui->stackedWidget->setCurrentIndex(4);
}

// --- LÓGICA DE GUARDADO ---
void Labo7::guardarContacto()
{
    QString nombre = ui->txtNombre->text().trimmed();
    QString telefono = ui->txtTelefono->text().trimmed();
    QString correo = ui->txtCorreo->text().trimmed();

    if (nombre.isEmpty() || telefono.isEmpty() || correo.isEmpty()) {
        QMessageBox::warning(this, "Datos incompletos", "Completá los tres campos.");
        return;
    }

    for (const QString& dato : QStringList{ nombre, telefono, correo }) {
        if (dato.contains(';') || dato.contains('\n') || dato.contains('\r')) {
            QMessageBox::warning(this, "Dato no permitido", "No usés punto y coma ni saltos de línea.");
            return;
        }
    }

    // CASO A: ESTAMOS EDITANDO UN CONTACTO EXISTENTE
    if (filaEditando >= 0) {
        // 1. Leer todas las líneas del archivo
        QFile archivo(archivoContactos.ruta());
        if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) return;

        QTextStream in(&archivo);
        QStringList lineas;
        int filaActual = 0;

        while (!in.atEnd()) {
            QString linea = in.readLine();
            if (!linea.trimmed().isEmpty()) {
                if (filaActual == filaEditando) {
                    // Reemplazamos la línea con los nuevos datos
                    lineas.append(nombre + ";" + telefono + ";" + correo);
                }
                else {
                    lineas.append(linea);
                }
                filaActual++;
            }
        }
        archivo.close();

        // 2. Sobrescribir el archivo con la línea modificada
        if (!archivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(this, "Error", "No se pudo actualizar el archivo.");
            return;
        }

        QTextStream out(&archivo);
        for (const QString& l : lineas) {
            out << l << "\n";
        }
        archivo.close();

        QMessageBox::information(this, "Actualizado", "Los cambios se guardaron correctamente.");
        filaEditando = -1; // Reseteamos el estado de edición
    }
    // CASO B: ES UN CONTACTO NUEVO
    else {
        if (!archivoContactos.guardar(nombre, telefono, correo)) {
            QMessageBox::critical(this, "No se pudo guardar", "Revisá permisos y espacio disponible.");
            return;
        }
        QMessageBox::information(this, "Guardado completo", "El contacto se guardó correctamente.");
    }

    // Limpiar campos y volver o refrescar
    ui->txtNombre->clear();
    ui->txtTelefono->clear();
    ui->txtCorreo->clear();
    ui->txtNombre->setFocus();
}

// --- CARGAR TABLA ---
void Labo7::cargarTablaContactos() {
    // 1. Configurar las columnas y cabeceras SIEMPRE
    ui->tableWidget->setColumnCount(3);
    QStringList headers = { "Nombre", "Teléfono", "Correo" };
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    // Limpiar filas anteriores y aplicar opciones de selección
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    // 2. Intentar abrir el archivo de contactos en modo lectura
    QFile archivo(archivoContactos.ruta());
    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&archivo);
    int fila = 0;

    // 3. Leer el archivo línea por línea si existe
    while (!in.atEnd()) {
        QString linea = in.readLine().trimmed();
        if (linea.isEmpty()) continue;

        QStringList partes = linea.split(";");

        if (partes.size() == 3) {
            ui->tableWidget->insertRow(fila);
            ui->tableWidget->setItem(fila, 0, new QTableWidgetItem(partes[0]));
            ui->tableWidget->setItem(fila, 1, new QTableWidgetItem(partes[1]));
            ui->tableWidget->setItem(fila, 2, new QTableWidgetItem(partes[2]));
            fila++;
        }
    }
    archivo.close();
}

// --- ELIMINAR DESDE EL BOTÓN INFERIOR ---
void Labo7::eliminarContacto() {
    // 1. Obtener la fila seleccionada en la tabla
    int filaSeleccionada = ui->tableWidget->currentRow();
    if (filaSeleccionada < 0) {
        QMessageBox::warning(this, "Sin selección", "Por favor, seleccioná un contacto de la tabla para eliminar.");
        return;
    }

    // 2. Pedir confirmación al usuario
    auto respuesta = QMessageBox::question(this, "Confirmar eliminación",
        "¿Estás seguro de que deseas eliminar este contacto?",
        QMessageBox::Yes | QMessageBox::No);

    if (respuesta != QMessageBox::Yes) {
        return;
    }

    // 3. Leer todas las líneas del archivo actual
    QFile archivo(archivoContactos.ruta());
    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&archivo);
    QStringList lineasRestantes;
    int filaActual = 0;

    while (!in.atEnd()) {
        QString linea = in.readLine();
        if (linea.trimmed().isEmpty()) continue;

        if (filaActual != filaSeleccionada) {
            lineasRestantes.append(linea);
        }
        filaActual++;
    }
    archivo.close();

    // 4. Reescribir el archivo completo con las líneas restantes
    if (!archivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "No se pudo actualizar el archivo.");
        return;
    }

    QTextStream out(&archivo);
    for (const QString& l : lineasRestantes) {
        out << l << "\n";
    }
    archivo.close();

    // 5. Refrescar la tabla
    cargarTablaContactos();

    QMessageBox::information(this, "Eliminado", "El contacto fue eliminado correctamente.");
}

// Métodos auxiliares para los botones inferiores (pueden llamar a eliminarContacto o lógica propia)
void Labo7::eliminarContactoSeleccionado() {
    eliminarContacto();
}

void Labo7::editarContactoSeleccionado() {
    // 1. Verificar si hay una fila seleccionada en la tabla
    int filaSeleccionada = ui->tableWidget->currentRow();
    if (filaSeleccionada < 0) {
        QMessageBox::warning(this, "Sin selección", "Por favor, seleccioná un contacto de la tabla para editar.");
        return;
    }

    // 2. Guardar el índice de la fila que vamos a editar
    filaEditando = filaSeleccionada;

    // 3. Extraer los datos de la tabla de esa fila
    QString nombre = ui->tableWidget->item(filaSeleccionada, 0)->text();
    QString telefono = ui->tableWidget->item(filaSeleccionada, 1)->text();
    QString correo = ui->tableWidget->item(filaSeleccionada, 2)->text();

    // 4. Pasar esos datos a los campos de texto del formulario (txtNombre, txtTelefono, txtCorreo)
    ui->txtNombre->setText(nombre);
    ui->txtTelefono->setText(telefono);
    ui->txtCorreo->setText(correo);

    // 5. Cambiar automáticamente a la página del formulario (ajusta el índice si tu página de registro es otra)
    ui->stackedWidget->setCurrentIndex(1);

    QMessageBox::information(this, "Modo Edición", "Modificá los datos en el formulario y guardá los cambios.");
}