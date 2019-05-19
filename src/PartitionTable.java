import java.io.IOException;
import java.io.RandomAccessFile;

/**
 * Wrapper para la tabla de particiones contenida en la
 * imagen de un disco duro, en este caso una tarjeta SD
 */
public class PartitionTable {

    private boolean foundFAT;

    public byte firstByte;
    public byte startCHS[];
    public byte partitionType;
    public byte endCHS[];
    public int startSector;
    public int lengthSector;

    public PartitionTable(RandomAccessFile img) throws IOException {
        // la tabla de particiones deberia de empezar en 0x1BE
        img.seek(0x1BE);
        this.startCHS = new byte[3];
        this.endCHS = new byte[3];
        this.foundFAT = false;

        // hay 4 entradas de la tabla de particiones, cada una de 16 bytes
        // una de estas particiones contiene el FAT FS
        for (int i = 0; i < 4; i += 1) {
            this.firstByte = img.readByte();
            img.read(this.startCHS);
            this.partitionType = img.readByte();
            img.read(this.endCHS);
            this.startSector = img.readInt();
            this.lengthSector = img.readInt();

            if (this.partitionType == 4 || this.partitionType == 6 || this.partitionType == 14) {
                // se encontro la FAT
                this.foundFAT = true;
                break;
            }
        }
    }

    public boolean hasFAT() {
        return this.foundFAT;
    }

}
