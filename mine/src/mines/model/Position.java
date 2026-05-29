package mines.model;

public record Position(int x, int y) {

    @Override
    public String toString() {
        return String.format("(%d, %d)", x, y);
    }

    @Override
    public final int hashCode() {
        int result = Integer.hashCode(x);
        result =  31 * result + Integer.hashCode(y);
        return result;
    }

    @Override
    public final boolean equals(Object o) {
        //check if is same address
        if (this == o) {
            return true;
        }
        //2.check if same class
        if (!(o instanceof Position(int x1, int y1))) {
            return false;
        }
        //here i wrote, now i think more clear and i ask AI don;t influence speed
        //if (!(o instanceof Position other))) return false;
        //return this.x == other.x && this.y == other.y
        //3.compare all components using "=="
        return this.x == x1 && this.y == y1;

    }

    public int x() {
        return x;
    }
    public int y() {
        return y;
    }
}
