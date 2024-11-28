using Npgsql;
using System;
using System.Windows.Forms;
using WinFormsApp1;

namespace WinFormsApp1
{
	public partial class TableSelect : Form
	{
		private bool _isEditable = false;
		public TableSelect(bool isEditable)
		{
			InitializeComponent();
			_isEditable = isEditable;
		}

		private void ClientButton_Click(object sender, EventArgs e)
		{
			Form1.ExecuteQuery("select \"Passport number\", \"Parktime\", \"Surname\", \"Name\", \"Otchestvo\" from \"Client\"", _isEditable, "Клиенты");
		}

		private void AutoButton_Click(object sender, EventArgs e)
		{
			Form1.ExecuteQuery("select \"Client number\", \"Auto number\", \"Mass\", \"Length\", \"Width\", \"Height\" from \"Auto\"", _isEditable, "Авто");
		}

		private void PlaceButton_Click(object sender, EventArgs e)
		{
			Form1.ExecuteQuery("select \"Place number\", \"Length\", \"Width\" from \"Place\"", _isEditable, "Места");
		}

		private void FloorButton_Click(object sender, EventArgs e)
		{
			Form1.ExecuteQuery("select \"Floor number\", \"Height\", \"Places amount\" from \"Floor\"", _isEditable, "Этажи");
		}

		private void ElevatorButton_Click(object sender, EventArgs e)
		{
			Form1.ExecuteQuery("select \"Elevator number\", \"Length\", \"Width\", \"Height\", \"lift capacity\" from \"Elevator\"", _isEditable, "Лифты");
		}

		private void TableButton_Click(object sender, EventArgs e)
		{
			Form1.ExecuteQuery("select \"Place numbers\", \"Place type\" from \"Table\"", _isEditable, "Табло");
		}

		private void ClientTableButton_Click(object sender, EventArgs e)
		{
			Form1.ExecuteQuery("select * from \"Client_Table\"", _isEditable, "Клиенты_Табло");
		}

		private void FloorElevatorButton_Click(object sender, EventArgs e)
		{
			Form1.ExecuteQuery("select * from \"Floor_Elevator\"", _isEditable, "Этажи_Лифты");
		}
	}
}
