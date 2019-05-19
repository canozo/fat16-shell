import java.io.IOException;
import java.io.RandomAccessFile;

/**
 * Wrapper para la imagen de FAT16
 * El objetivo es obtener toda la informacion que se lista
 * en la especificacion (specs.pdf) y guardar la informacion en
 * la clase para su facil uso
 */
public class FAT16 {

    private RandomAccessFile img;
    private PartitionTable partitionTable;

    // apuntadores a Root y Data
    private int firstRootSector;
    private int firstDataSector;

    // Boot Sector (BS) y BIOS parameter block (BPB)
    private short BPB_BytsPerSec;
    private int BPB_SecPerClus;
    private int BPB_RsvdSecCnt;
    private int BPB_NumFATs;
    private int BPB_RootEntCnt;
    private int BPB_TotSec16;
    private int BPB_Media;
    private int BPB_FATSz16;
    private int BPB_SecPerTrk;
    private int BPB_NumHeads;
    private int BPB_HiddSec;
    private int BPB_TotSec32;
    private int BS_DrvNum;
    private int BS_Reserved1;
    private int BS_BootSig;
    private int BS_VolID;
    private int BS_VolLab;
    private int BS_FilSysType;

    public FAT16(RandomAccessFile img) throws IOException {
        this.img = img;
        this.partitionTable = new PartitionTable(this.img);
        if (this.partitionTable.hasFAT()) {
            System.out.println("Inicio del sector: " + String.format("%08X", this.partitionTable.startSector));
            System.out.println("Longitud del sector: " + String.format("%d", this.partitionTable.lengthSector));
        }
    }
}
