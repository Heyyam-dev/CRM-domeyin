using Microsoft.EntityFrameworkCore;
using XaApi.Models;

namespace XaApi.Data;

public class AppDbContext : DbContext
{
    public AppDbContext(DbContextOptions<AppDbContext> options) : base(options) { }

    public DbSet<Contact> Contacts => Set<Contact>();
}
