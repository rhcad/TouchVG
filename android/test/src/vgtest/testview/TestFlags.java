package vgtest.testview;

public class TestFlags {
    public final static int SPLINES_CMD = 1 << 1;
    public final static int LINE_CMD = 2 << 1;
    public final static int LINES_CMD = 3 << 1;
    public final static int HITTEST_CMD = 4 << 1;
    public final static int SELECT_CMD = 1;
    public final static int TWO_MAGVIEWS = 16;
    public final static int CMD_MASK = 15;
    public final static int RAND_SHAPES = 32;
    public final static int RECORD = 64;
    public final static int PLAY_SHAPES = 64;
    public final static int HAS_BACKDRAWABLE = 128;

    public final static int OPAQUE_VIEW = 0x1000;
    public final static int LARGE_VIEW = 0x10000;
    public final static int LARGE_SURFACEVIEW = 0x20000;

    public final static int MODEL_SURFACE = 0x100000;
}
