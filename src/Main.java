import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.Scanner;

public class Main {

    public static void main(String[] args) {
        // args[0]: nombre de la imagen
        try {
            RandomAccessFile img = new RandomAccessFile(new File(args[0]), "rw");
            shell(img);
            img.close();
        } catch (FileNotFoundException e) {
            System.out.println("No se encontro la imagen \"" + args[0] + "\"");
        } catch (IOException e) {
            System.out.println("Error: IOException\n" + e);
        }
    }

    private static void shell(RandomAccessFile img) {
        // shell
        String[] commands;
        Scanner cin = new Scanner(System.in);

        while (true) {
            commands = cin.nextLine().trim().split("\\s+");

            // salir si ingreso "exit"
            if (commands[0].equals("exit")) {
                System.out.println("Saliendo...");
                break;
            }

            // validar sintaxis correcta y ejecutar
            if (validateCommand(commands)) {
                switch (commands[0]) {
                    case "ls":
                        // ls -l
                        System.out.println("ls -l");
                        break;
                    case "cat":
                        if (commands.length == 2) {
                            // cat archivo
                            System.out.println("cat archivo");
                        } else {
                            // cat > archivo.txt
                            System.out.println("cat > archivo.txt");
                        }
                        break;
                    case "mkdir":
                        // mkdir DIR
                        System.out.println("mkdir DIR");
                        break;
                    case "cd":
                        // cd DIR
                        System.out.println("cd DIR");
                        break;
                    default:
                        // ???
                        System.out.println("??? inesperado");
                        break;
                }
            }


        }
    }

    private static boolean validateCommand(String[] commands) {
        // verificar que ingreso un comando valido
        if (commands.length < 2) {
            System.out.println("Ingreso muy pocos argumentos.");
            return false;
        }

        if (commands.length > 3) {
            System.out.println("Ingreso muchos argumentos.");
            return false;
        }

        // verificar primer comando
        switch (commands[0]) {
            case "ls":
            case "cat":
            case "mkdir":
            case "cd":
                break;
            default:
                System.out.println("Argumento \"" + commands[0] + "\" no valido.");
                return false;
        }

        // ls -l
        if (commands[0].equals("ls") && !commands[1].equals("-l")) {
            System.out.println("Argumento \"" + commands[1] + "\" no valido.");
            return false;
        }

        // cat > archivo.txt
        if (commands.length == 3) {
            if (!commands[0].equals("cat")) {
                System.out.println("Muchos argumentos para \"" + commands[0] + "\".");
                return false;
            }

            if (!commands[1].equals(">")) {
                System.out.println("Argumento \"" + commands[1] + "\" no valido.");
            }
        }

        // sintaxis correcta
        return true;
    }
}
