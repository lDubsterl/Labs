import com.mysql.jdbc.exceptions.MySQLIntegrityConstraintViolationException

import java.awt.event.{ActionEvent, ActionListener}
import javax.swing.{JButton, JFrame, JLabel, JPanel, JTextArea, JTextField, SwingUtilities}
import java.awt.Dimension
import java.sql.{Connection, DriverManager, ResultSet}
import java.awt.Color

object ButtonModule {
  val frame = new JFrame("My Application")
  frame.setSize(800, 600)
  frame.setLayout(null)
  frame.getContentPane().setBackground(Color.CYAN)
  frame.setLocationRelativeTo(null)
  frame.setVisible(true)

  def main(args: Array[String]): Unit = {
    SwingUtilities.invokeLater(() => {

      // val panel = new JPanel()
      val label1 = new JLabel("Title")
      val label2 = new JLabel("Price")
      val amountLabel = new JLabel("Amount")
      label1.setBounds(235, 120, 100, 20)
      label2.setBounds(375, 120, 100, 20)
      amountLabel.setBounds(510, 120, 100, 20)
      val setButton = new JButton("Insert")
      val selectButton = new JButton("Select")
      val groupButton = new JButton("Group by product")
      val likeButton = new JButton("Select like")
      val nameField = new JTextField(20)
      val priceField = new JTextField(20)
      val amountField = new JTextField(20)
      val infoField = new JTextArea()
      setButton.setBackground(Color.BLUE)
      setButton.setForeground(Color.PINK)
      setButton.setBounds(105, 80, 120, 20)
      selectButton.setBounds(255, 80, 120, 20)
      groupButton.setBounds(405, 80, 150, 20)
      likeButton.setBounds(585, 80, 150, 20)
      nameField.setBounds(200, 150, 100, 20)
      priceField.setBounds(340, 150, 100, 20)
      amountField.setBounds(480, 150, 100, 20)
      infoField.setBounds(200, 200, 400, 200)
      frame.add(setButton)
      frame.add(selectButton)
      frame.add(label1)
      frame.add(nameField)
      frame.add(label2)
      frame.add(priceField)
      frame.add(amountLabel)
      frame.add(amountField)
      frame.add(infoField)
      frame.add(groupButton)
      frame.add(likeButton)
      setButton.addActionListener(new ActionListener {
        override def actionPerformed(e: ActionEvent): Unit = {
          val url = "jdbc:mysql://localhost:3306/mysql"
          val username = "root"
          val password = "root"
          Class.forName("com.mysql.jdbc.Driver")
          // Class.forName("org.gjt.mm.mysql.Driver")

          val conn = DriverManager.getConnection(url, username, password)

          try {
            val stmt = conn.createStatement()
            val rs = stmt.execute("INSERT INTO podval VALUES ('" + nameField.getText + "'," + priceField.getText + "," + amountField.getText + ")")
            priceField.setText("")
            nameField.setText("")
            amountField.setText("")
          } finally {
            conn.close()
          }
        }
      }
      )
      selectButton.addActionListener(new ActionListener {
        override def actionPerformed(e: ActionEvent): Unit = {
          ////////
          {
            val url = "jdbc:mysql://localhost:3306/mysql"
            val username = "root"
            val password = "root"

            Class.forName("com.mysql.jdbc.Driver")
            val conn = DriverManager.getConnection(url, username, password)

            try {
              val stmt = conn.createStatement()
              val prod_name = nameField.getText().trim()
              var rs: java.sql.ResultSet = null
              if (!prod_name.isBlank && priceField.getText.isBlank && amountField.getText.isBlank) {
                rs = stmt.executeQuery("SELECT * FROM podval WHERE product = '" + prod_name + "'")
                while (rs.next()) {
                  val name = rs.getString("product")
                  val price = rs.getInt("price")
                  val amount = rs.getInt("amount")
                  priceField.setText("" + price)
                  amountField.setText("" + amount)
                }
              }
              if (prod_name.isBlank && !priceField.getText.isBlank && amountField.getText.isBlank) {
                rs = stmt.executeQuery("select * from podval where price >= ALL(select price from podval)")
                infoField.setText("product\tprice\tamount\n")
                while (rs.next()) {
                  val name = rs.getString("product")
                  val price = rs.getString("price")
                  val amount = rs.getString("amount")
                  infoField.setText(infoField.getText + name + "\t" + price + "\t" + amount + "\n")
                }
              }
              if (prod_name.isBlank && priceField.getText.isBlank && amountField.getText.isBlank) {
                rs = stmt.executeQuery("SELECT product, max(price) as price, max(amount) as amount  FROM podval  GROUP BY  product")
                while (rs.next()) {
                  val name = rs.getString("product")
                  val price = rs.getString("price")
                  val amount = rs.getString("amount")
                    nameField.setText(name)
                    priceField.setText(price)
                    amountField.setText(amount)

                }
              }
            }
            finally {
              conn.close()
            }
          }
        }
      }
      )
      groupButton.addActionListener(new ActionListener {
        override def actionPerformed(e: ActionEvent): Unit = {
          val url = "jdbc:mysql://localhost:3306/mysql"
          val username = "root"
          val password = "root"

          Class.forName("com.mysql.jdbc.Driver")
          val conn = DriverManager.getConnection(url, username, password)

          try {
            val stmt = conn.createStatement()
            val prod_name = nameField.getText().trim()
            var rs: java.sql.ResultSet = null
            rs = stmt.executeQuery("SELECT product, AVG(price) as price, SUM(amount) as amount FROM podval2 group by product")
            infoField.setText("product\tprice\tamount\n")
            while (rs.next()) {
              val name = rs.getString("product")
              val price = rs.getInt("price")
              val amount = rs.getInt("amount")
              infoField.setText(infoField.getText + name + "\t" + price + "\t" + amount + "\n")
            }
          } finally {
            conn.close()
          }
        }
      }
      )
      likeButton.addActionListener(new ActionListener {
        override def actionPerformed(e: ActionEvent): Unit = {
          val url = "jdbc:mysql://localhost:3306/mysql"
          val username = "root"
          val password = "root"

          Class.forName("com.mysql.jdbc.Driver")
          val conn = DriverManager.getConnection(url, username, password)

          try {
            val stmt = conn.createStatement()
            val prod_name = nameField.getText().trim()
            var rs: java.sql.ResultSet = null
            rs = stmt.executeQuery("SELECT * FROM podval WHERE product LIKE '" + prod_name + "%'")
            infoField.setText("product\tprice\tamount\n")
            while (rs.next()) {
              val name = rs.getString("product")
              val price = rs.getInt("price")
              val amount = rs.getInt("amount")
              infoField.setText(infoField.getText + name + "\t" + price + "\t" + amount + "\n")
            }
          } finally {
            conn.close()
          }
        }
      }
      )
    }
    )
  }
}