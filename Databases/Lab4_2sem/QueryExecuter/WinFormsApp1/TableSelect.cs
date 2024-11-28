using MongoDB.Driver;
using System;
using System.Windows.Forms;
using WinFormsApp1;

namespace WinFormsApp1
{
	public partial class TableSelect : Form
	{
		private bool _isEditable = false;
		private IMongoDatabase _db;
		public TableSelect(bool isEditable, IMongoDatabase db)
		{
			InitializeComponent();
			_isEditable = isEditable;
			_db = db;
		}

		private void ClientButton_Click(object sender, EventArgs e)
		{
			Form1.ExecuteQuery(_isEditable, _db, "Client", "Клиенты");
		}

		private void AutoButton_Click(object sender, EventArgs e)
		{
			Form1.ExecuteQuery(_isEditable, _db, "Auto", "Авто");
		}

		private void PlaceButton_Click(object sender, EventArgs e)
		{
			Form1.ExecuteQuery(_isEditable, _db, "Place", "Места");
		}

		private void FloorButton_Click(object sender, EventArgs e)
		{
			Form1.ExecuteQuery(_isEditable, _db, "Floor", "Этажи");
		}

		private void ElevatorButton_Click(object sender, EventArgs e)
		{
			Form1.ExecuteQuery(_isEditable, _db, "Elevator", "Лифты");
		}

		private void TableButton_Click(object sender, EventArgs e)
		{
			Form1.ExecuteQuery(_isEditable, _db, "Table", "Табло");
		}

		private void ClientTableButton_Click(object sender, EventArgs e)
		{

		}

		private void FloorElevatorButton_Click(object sender, EventArgs e)
		{

		}
	}
}
