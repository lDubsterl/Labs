using System;
using System.Data;
using System.Windows.Forms;
using MongoDB.Bson;
using MongoDB.Driver;

namespace WinFormsApp1
{
	public static class BsonElementExtension
	{
		public static Type ToType(this BsonType type)
		{
			switch (type)
			{
				case BsonType.String: return typeof(string);
				case BsonType.Boolean: return typeof(bool);
				case BsonType.Int32: return typeof(int);
				case BsonType.Int64: return typeof(long);
				case BsonType.Double: return typeof(double);
				default: return typeof(object);
			}
		}
	}
	public partial class Form1 : Form
	{
		public Form1()
		{
			InitializeComponent();
		}

		public static MongoClient conn = new("mongodb://localhost:27017");

		public static IMongoDatabase db = conn.GetDatabase("Parking");

		public static void ExecuteQuery(bool isEditable, IMongoDatabase db, string collectionName, string tableName = "")
		{
			var collection = db.GetCollection<BsonDocument>(collectionName);
			var docs = collection.Find(FilterDefinition<BsonDocument>.Empty);
			var table = new DataTable();

			foreach (var column in docs.First())
			{
				if (!column.Name.Contains("id"))
					table.Columns.Add(column.Name, column.Value.BsonType.ToType());
			}
			foreach (var doc in docs.ToList())
			{
				var row = table.NewRow();
				foreach (var column in doc)
				{
					if (!column.Name.Contains("id"))
						row[column.Name] = column.Value;
				}
				table.Rows.Add(row);
			}

			if (isEditable)
			{
				var resultForm = new ResultTableEditable(table);
				resultForm.Text = tableName;
				resultForm.Show();
			}
			else
			{
				var resultForm = new ResultTable(table);
				resultForm.Show();
			}
		}

		private void ExecuteButton_Click(object sender, EventArgs e)
		{
			TableSelect form = new(true, db);
			form.Show();
		}

		private void ShowTablesButton_Click(object sender, EventArgs e)
		{
			TableSelect form = new(false, db);
			form.Show();
		}
	}
}
