import scala.Int.int2long

object Main {

  private def substringFromSymbolsAtPositions(str: String, bit: Int): String = {
    val result = new StringBuilder;
    for (i <- str.indices if i % 2 == bit) {
      result.append(str(i));
    }
    result.toString();
  }

  def main(args: Array[String]): Unit = {
    print("Enter the number: ")
    var numberString = ""
    var a = 0
    var b = 0
    var c = 0
    var rez: Float = 0
    var z: Int = 0
    numberString = scala.io.StdIn.readLine()
    if ((numberString.length() != 10)) {
      println("Incorrect number");
      sys.exit(0)
    }

    for (i <- 1 to 5) {
      if (numberString.length() < 10)
        numberString = numberString.appended('0');
      a = substringFromSymbolsAtPositions(numberString, 0).toInt;

      b = substringFromSymbolsAtPositions(numberString, 1).toInt;
      c = a * b
      rez = c.toString().substring(0, 3).toFloat / 1000
      z += c
      if (z.toString().substring(0, 1).equals("-"))
        z *= -1;
      println(a);
      println(b);
      println(c);
      println(rez);
      println("z=" + z)
      numberString = numberString.substring(1, numberString.length() - 1)
      numberString = "011" + z
    }
  }
}