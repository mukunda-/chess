#!/usr/bin/env tsx

import { sql } from "drizzle-orm";
import { db } from "@/db/db";

async function main() {
  const [, , table] = process.argv;
  if (!table) {
    console.error("Expected table to drop");
    return;
  }

  const query = sql.raw(`DROP TABLE "${table}" CASCADE;`);
  await db.execute(query);
}

main();
