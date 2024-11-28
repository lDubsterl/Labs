using System;
using System.Windows.Forms;
using Npgsql;

namespace WinFormsApp1
{
	public partial class Form1 : Form
	{
		public Form1()
		{
			InitializeComponent();
			conn.Open();
		}

		public static ApplicationContext db = new ApplicationContext();

		public static NpgsqlConnection conn = new NpgsqlConnection("Server=127.0.0.1;User Id=postgres;Password=1;Port=5432;Database=Parking;");

		public static void ExecuteQuery(string sqlCommand, bool isEditable = false, string tableName = "")
		{
			NpgsqlDataAdapter adapter = new(sqlCommand, conn);
			if (isEditable)
			{
				var resultForm = new ResultTableEditable(adapter);
				resultForm.Text = tableName;
				resultForm.Show();
			}
			else
			{
				var resultForm = new ResultTable(adapter);
				resultForm.Show();
			}
		}

		private void ExecuteButton_Click(object sender, EventArgs e)
		{
			TableSelect form = new(true);
			form.Show();
		}

		private void ShowTablesButton_Click(object sender, EventArgs e)
		{
			TableSelect form = new(false);
			form.Show();
		}
	}
}
