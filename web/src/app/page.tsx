// NextJS
import { redirect } from "next/navigation";

// Ours - Auth
import { getServerSession } from "@/server/session";

import { Stack } from "@mui/material";
import Game from "@/components/Game";

export default async function Page() {
  const session = await getServerSession();
  if (session) {
    redirect("/user");
  }

  return (
    <Stack>
      <Game />
    </Stack>
  );
}
