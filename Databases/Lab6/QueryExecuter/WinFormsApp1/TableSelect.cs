using Npgsql;
using System;
using System.Windows.Forms;
using WinFormsApp1;

namespace WinFormsApp1
{
	public partial class TableSelect : Form
	{
		public TableSelect()
		{
			InitializeComponent();
		}

		private void ClientButton_Click(object sender, EventArgs e)
		{
			Form1.ExecuteQuery("select * from \"Client\"", true, "Клиенты");
		}

		private void AutoButton_Click(object sender, EventArgs e)
		{
			Form1.ExecuteQuery("select * from \"Auto\"", true, "Авто");
		}

		private void PlaceButton_Click(object sender, EventArgs e)
		{
			Form1.ExecuteQuery("select * from \"Place\"", true, "Места");
		}

		private void FloorButton_Click(object sender, EventArgs e)
		{
			Form1.ExecuteQuery("select * from \"Floor\"", true, "Этажи");
		}

		private void ElevatorButton_Click(object sender, EventArgs e)
		{
			Form1.ExecuteQuery("select * from \"Elevator\"", true, "Лифты");
		}

		private void TableButton_Click(object sender, EventArgs e)
		{
			Form1.ExecuteQuery("select * from \"Table\"", true, "Табло");
		}

		private void ClientTableButton_Click(object sender, EventArgs e)
		{
			Form1.ExecuteQuery("select * from \"Client_Table\"", true, "Клиенты_Табло");
		}

		private void FloorElevatorButton_Click(object sender, EventArgs e)
		{
			Form1.ExecuteQuery("select * from \"Floor_Elevator\"", true, "Этажи_Лифты");
		}
	}
}
